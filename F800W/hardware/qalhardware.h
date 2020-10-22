#ifndef QALHARDWARE_H
#define QALHARDWARE_H
#include <QThread>
#include <QMutex>
#include <QTimer>
#include <QSemaphore>
#include <QProcess>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <linux/input.h>

#include "countdown.h"
#include "sample_vio.h"

#define OFF 0
#define ON 1
#define GREEN 2
#define RED 3
#define IR_WHITE 0xff

#define GPIO_CMD_MAGE   'G'
#define GPIO_PWM0_SET   _IOW(GPIO_CMD_MAGE,0,char)
#define LCD_PWM1_SET    _IOW(GPIO_CMD_MAGE,1,char)
#define GPIO_SET        _IOW(GPIO_CMD_MAGE,2,char)
#define BELL_GET        _IOW(GPIO_CMD_MAGE,3,char)
#define DET_GET         _IOW(GPIO_CMD_MAGE,4,char)
#define LED_B_PWM_SET   _IOW(GPIO_CMD_MAGE,5,char)
#define LEDG_PWM0_SET   _IOW(GPIO_CMD_MAGE,6,char)
#define RF_GET          _IOR(GPIO_CMD_MAGE,7,char)
#define CSN0_SET        _IOW(GPIO_CMD_MAGE,8,char)
#define LED_G_SET       _IOW(GPIO_CMD_MAGE,9,char)
#define LED_R_SET       _IOW(GPIO_CMD_MAGE,10,char)
#define RELAY_SET       _IOW(GPIO_CMD_MAGE,11,char)
#define MOTOR_SET       _IOW(GPIO_CMD_MAGE,12,char) //韦根
#define MUTE_SET        _IOW(GPIO_CMD_MAGE,13,char)
#define IR_SET          _IOW(GPIO_CMD_MAGE,15,char)
#define WHITE_SET       _IOW(GPIO_CMD_MAGE,16,char)
#define TGPIO_GET       _IOR(GPIO_CMD_MAGE,17,char)
#define WDG_SET         _IOW(GPIO_CMD_MAGE,18,char)
#define WDG_F           _IOW(GPIO_CMD_MAGE,19,char)
#define CARD_EN         _IOW(GPIO_CMD_MAGE,20,char)
#define G4_RESET        _IOW(GPIO_CMD_MAGE,21,char)

struct led_pwm_set{
    int w;                 // work or dont work
    int opentime;          //呼吸灯亮的时间
    int closetime;          //  呼吸灯灭的时间，为0的时候，呼吸灯一直循环，没有灭掉间隔
    int res;
};

// gpio驱动接口 - 单例模式
class QalHardWare : public QThread
{
public:
    explicit QalHardWare();
    static QalHardWare *getInstance()
    {
        if (!m_Instance) {
            m_Instance = new QalHardWare();
            m_Instance->start();
        }
        return m_Instance;
    }
    // 设置音量值，0-100
    void setVolume(int vol);
    // 呼吸灯控制
    void ctlBreatheLed(int ctl);
    void ctlBLN(int ctl);
    // 红外灯控制
    void ctlIr(int ctl);
    // 看门狗 喂狗，否则3秒自动重启
    void ctlWDG();
    // 补光灯控制
    void ctlWhite(int ctl);
    // 补光灯和红外灯同步
    void ctlIrWhite(int ctl);
    // 控制屏幕亮度 亮度范围0-100; 0的时候关闭背光，100为最亮
    void ctlBacklight(int value);
    // 检测是否关门
    void checkCloseDoor();
    // 检测是否关门
    void checkOpenDoor();
    // 音频播放接口 支持tts播放和aac语音文件播放 前者播放对应content内容，后者对应filename文件名
    void playSound(const char *content, const QString &filename);
    // 播放aac语音文件
    void playSoundAac(int tupe, const QString &filename);
    // 播放tts翻译
    void playSoundTTS(const char *content);

protected:
    virtual void run();

private:
    void init();
    void ttsInit();
    int text_to_speech(const char* src_text, const char* des_path, const char* params);

private:
    static QalHardWare *m_Instance;
    led_pwm_set m_led;
    CountDown m_timer;
    int m_fd = 0;
    int m_i = 3;
    QMutex m_mutex;
};


#endif // QALHARDWARE_H
