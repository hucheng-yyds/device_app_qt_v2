#include <QFile>
#include "qalhardware.h"
#include "msp_cmn.h"
#include "msp_errors.h"
#include "msp_types.h"
#include "qtts.h"
#include <QDebug>

QalHardWare* QalHardWare::m_Instance = nullptr;

QalHardWare::QalHardWare()
{
    moveToThread(this);
}

void QalHardWare::setVolume(int vol)
{
    if(0 == vol)
    {
        vol = -106;
    }
    else {
        vol = vol / 5 - 14;
    }
    IF_Vol_Set(vol);
}

void QalHardWare::run()
{
    m_fd = open("/dev/mypwm", O_RDWR);
    m_led.opentime=2;
    m_led.closetime=0;
    countdown_ms(0);
//    ioctl(m_fd, WDG_SET, &m_i);
    init();
    exec();
}

bool QalHardWare::expired()
{
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time =QDateTime::currentDateTime();
    qint64 now = origin_time.msecsTo(current_date_time);
    if(now >= m_endTimerMs)
    {
        return true;
    }
    else {
        return false;
    }
}

void QalHardWare::countdown_ms(int ms)
{
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time =QDateTime::currentDateTime();
    qint64 now = origin_time.msecsTo(current_date_time);
    m_endTimerMs = now + ms;
}

void QalHardWare::ctlLed(int ctl)
{
    switch (ctl) {
    case OFF:
        m_i=0;
        ioctl(m_fd,LED_G_SET,&m_i);
        m_i=0;
        ioctl(m_fd,LED_R_SET,&m_i);
        break;
    case GREEN:
        ctlBLN(OFF);
        m_i=1;
        ioctl(m_fd,LED_G_SET,&m_i);
        m_i=0;
        ioctl(m_fd,LED_R_SET,&m_i);
        break;
    case RED:
        ctlBLN(OFF);
        m_i=0;
        ioctl(m_fd,LED_G_SET,&m_i);
        m_i=1;
        ioctl(m_fd,LED_R_SET,&m_i);
        break;
    default:
        break;
    }
}

void QalHardWare::ctlBLN(int ctl)
{
    switch (ctl) {
    case OFF:
        m_led.w=0;
        ioctl(m_fd,LED_B_PWM_SET,&m_led);
        break;
    case ON:
        m_led.w=1;
        ioctl(m_fd,LED_B_PWM_SET,&m_led);
        break;
    default:
        break;
    }
}

void QalHardWare::ctlWDG()
{
    ioctl(m_fd,WDG_F,&m_i);
}

void QalHardWare::ctlIr(int ctl)
{
    ioctl(m_fd,IR_SET,&ctl);
}

void QalHardWare::ctlWhite(int ctl)
{
    ioctl(m_fd,WHITE_SET,&ctl);
}

void QalHardWare::ctlIrWhite(int ctl)
{
    ioctl(m_fd,IR_SET,&ctl);
    ioctl(m_fd,WHITE_SET,&ctl);
}

void QalHardWare::ctlBacklight(int value)
{
    ioctl(m_fd,LCD_PWM1_SET,&value);
}

void QalHardWare::checkCloseDoor()
{
    if (expired()) {
        m_i = 0;
        ioctl(m_fd, RELAY_SET, &m_i);
        ctlLed(OFF);
        ctlBLN(ON);
        ctlIrWhite(OFF);
    }
}

void QalHardWare::checkOpenDoor()
{
    if (expired()) {
        m_i = 1;
        ioctl(m_fd, RELAY_SET, &m_i);
        countdown_ms(switchCtl->m_doorDelayTime * 1000);
    }
}

void QalHardWare::ttsInit()
{
    const char* login_params = "appid = 5e5c927a, work_dir = .";//登录参数,appid与msc库绑定,请勿随意改动
    /* 用户登录 */
    int ret = MSPLogin(nullptr, nullptr, login_params); //第一个参数是用户名，第二个参数是密码，第三个参数是登录参数，用户名和密码可在http://www.xfyun.cn注册获取
    if (MSP_SUCCESS != ret)
    {
        printf("MSPLogin failed, error code: %d.\n", ret);
    }
}

void QalHardWare::init()
{
    if(!switchCtl->m_tts)
    {
        IF_AUDIO_Init();
    }
    else {
        IF_PCMAUDIO_Init();
        ttsInit();
    }
    setVolume(switchCtl->m_volume);
}

int QalHardWare::text_to_speech(const char* src_text, const char* des_path, const char* params)
{
    int          ret          = -1;
    FILE*        fp           = nullptr;
    const char*  sessionID    = nullptr;
    unsigned int audio_len    = 0;
//    wave_pcm_hdr wav_hdr      = default_wav_hdr;
    int          synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;

    if (nullptr == src_text || nullptr == des_path)
    {
        printf("params is error!\n");
        return ret;
    }
    fp = fopen(des_path, "wb+");
    if (nullptr == fp)
    {
        printf("open %s error.\n", des_path);
        return ret;
    }
    /* 开始合成 */
    sessionID = QTTSSessionBegin(params, &ret);
    if (MSP_SUCCESS != ret)
    {
        printf("QTTSSessionBegin failed, error code: %d.\n", ret);
        fclose(fp);
        return ret;
    }
    ret = QTTSTextPut(sessionID, src_text, (unsigned int)strlen(src_text), nullptr);
    if (MSP_SUCCESS != ret)
    {
        printf("QTTSTextPut failed, error code: %d.\n",ret);
        QTTSSessionEnd(sessionID, "TextPutError");
        fclose(fp);
        return ret;
    }
    while (1)
    {
        /* 获取合成音频 */
        const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
        if (MSP_SUCCESS != ret)
            break;
        if (nullptr != data)
        {
            fwrite(data, audio_len, 1, fp);
        }
        if (MSP_TTS_FLAG_DATA_END == synth_status)
            break;
    }
//    qt_debug() << printf("合成完毕\n");
    if (MSP_SUCCESS != ret)
    {
        printf("QTTSAudioGet failed, error code: %d.\n",ret);
        QTTSSessionEnd(sessionID, "AudioGetError");
        fclose(fp);
        return ret;
    }

    /* 合成完毕 */
    ret = QTTSSessionEnd(sessionID, "Normal");
    if (MSP_SUCCESS != ret)
    {
        printf("QTTSSessionEnd failed, error code: %d.\n",ret);
    }

    char buf[160];
    int size;
    fseek(fp, 0, SEEK_SET);
//    qt_debug() << "send start !";
    int ind = 1;
    do {
        size = fread(buf, 1, 160, fp);
        IF_PCMAUDIO_SendData(size, buf, ind ++);
    } while (size);
//    qt_debug() << "send over !";
    fclose(fp);
    fp = nullptr;

    return ret;
}

void QalHardWare::playSound(const char *content, const QString &filename)
{
    int language = switchCtl->m_language;
    int playType = !switchCtl->m_tts;
    if(language != 0 && language != 1)
    {
        playSoundAac(language, filename);
    }
    else {
        if(playType)
        {
            playSoundAac(language, filename);
        }
        else {
            playSoundTTS(content);
        }
    }
}

void QalHardWare::playSoundAac(int type, const QString &filename)
{
    m_mutex.lock();
    IF_AUDIO_ClearBuf();
    char buf[160];
    int vol = 1;
    int size;
    QString name = QString("aac%1/%2").arg(type).arg(filename);
    QFile file(name);
    ioctl(m_fd, MUTE_SET, &vol);
    if (!file.open(QIODevice::ReadOnly)) {
        m_mutex.unlock();
        return ;
    }
    qDebug() << "send start !";
    do {
        size = file.read(buf, 160);
        if(size > 0)
        {
            IF_AUDIO_SendData(size, buf);
        }
    } while (size);
    qDebug() << "send over !";
    file.close();
    msleep(400);
    m_mutex.unlock();
}

void QalHardWare::playSoundTTS(const char *content)
{
    m_mutex.lock();
    IF_AUDIO_ClearBuf();
    int vol = 1;
    std::string std_text = content;
    std::string voice_name;
    if (1) {
        voice_name = "xiaoyan";
    } else {
        voice_name = "catherine";
        std_text = "[g2]" + std_text;
    }

    int         ret                  = MSP_SUCCESS; // catherine xiaoyuan
    std::string params = "engine_type = local,voice_name="+ voice_name +", text_encoding = UTF8,"
                                "tts_res_path = fo|res/tts/"+ voice_name +".jet;fo|res/tts/common.jet,"
                                "sample_rate = 16000, speed = 70, volume = 100, pitch = 50, rdn = 0";
    const char* session_begin_params = params.data();
    const char* filename             = "tts_sample.wav"; //合成的语音文件名称
    const char* text                 = std_text.data(); //合成文本

    /* 文本合成 */
    printf("开始合成 ...\n");
    ioctl(m_fd, MUTE_SET, &vol);
    ret = text_to_speech(text, filename, session_begin_params);
    if (MSP_SUCCESS != ret)
    {
        printf("text_to_speech failed, error code: %d.\n", ret);
    }
    m_mutex.unlock();
}
