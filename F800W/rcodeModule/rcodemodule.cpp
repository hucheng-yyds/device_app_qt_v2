#include <QDateTime>
#include "rcodemodule.h"
#include "sqldatabase.h"
#include "qalhardware.h"
#include "httpsclient.h"

static ushort CRC16_CCITT(QByteArray content)
{
    content.replace(",", "");
    content.remove(content.size() - 4, 4);
    content = QByteArray::fromHex(content);
    content.append(0x01);
    content.append(0x02);
    content.append(0x03);
    content.append(0x04);
    const uchar *nData = (const uchar*)content.data();
    ushort wLength = content.size();
    static const ushort table[] = { 0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601, 0x06C0, 0x0780, 0xC741,
            0x0500, 0xC5C1, 0xC481, 0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40, 0x0A00, 0xCAC1, 0xCB81, 0x0B40,
            0xC901, 0x09C0, 0x0880, 0xC841, 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1, 0xDF81, 0x1F40,
            0xDD01, 0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641, 0xD201, 0x12C0, 0x1380, 0xD341,
            0x1100, 0xD1C1, 0xD081, 0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740,
            0xF501, 0x35C0, 0x3480, 0xF441, 0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41,
            0x3900, 0xF9C1, 0xF881, 0x3840, 0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41,
            0x2D00, 0xEDC1, 0xEC81, 0x2C40, 0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340,
            0xE101, 0x21C0, 0x2080, 0xE041, 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740,
            0xA501, 0x65C0, 0x6480, 0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 0xAA01, 0x6AC0, 0x6B80, 0xAB41,
            0x6900, 0xA9C1, 0xA881, 0x6840, 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41, 0xBE01, 0x7EC0, 0x7F80, 0xBF41,
            0x7D00, 0xBDC1, 0xBC81, 0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340,
            0xB101, 0x71C0, 0x7080, 0xB041, 0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241, 0x9601, 0x56C0, 0x5780, 0x9741,
            0x5500, 0x95C1, 0x9481, 0x5440, 0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40,
            0x9901, 0x59C0, 0x5880, 0x9841, 0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40,
            0x8D01, 0x4DC0, 0x4C80, 0x8C41, 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641, 0x8201, 0x42C0, 0x4380, 0x8341,
            0x4100, 0x81C1, 0x8081, 0x4040, };

    ushort crc = 0x0000;
    while (wLength--) {
        crc = (crc >> 8) ^ table[(crc ^ *nData) & 0xff];
        nData++;
    }
    return crc;
}

static int setOpt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio,oldtio;
    /*save local informion of uart,if uart port error,then about it informion be appeard*/
    if(tcgetattr(fd,&oldtio)!=0){
        perror("SetupSerial 1");
        return -1;
    }
    bzero(&newtio,sizeof(newtio));
    /*step 1,set fontsize*/
    newtio.c_cflag |=CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;
    /*set bit_stop*/
    switch(nBits)
    {
    case 7:
        newtio.c_cflag |=CS7;
        break;
    case 8:
        newtio.c_cflag |=CS8;
        break;
    default:
        newtio.c_cflag |=CS8;
        break;
    }
    /*set jo_check_bit*/
    switch(nEvent)
    {
    case 'O'://j
        newtio.c_cflag |=PARENB;
        newtio.c_cflag |=PARODD;
     //   newtio.c_iflag |=(INPCK | ISTRIP);
        break;
    case 'E'://O
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N'://no
//    		newtio.c_iflag &= ~INPCK ;  //
        newtio.c_cflag &=~PARENB;
        break;
    }
    /*set bps*/
    switch(nSpeed)
    {
        case 2400:
            cfsetispeed(&newtio,B2400);
            cfsetospeed(&newtio,B2400);
            break;
        case 4800:
            cfsetispeed(&newtio,B4800);
            cfsetospeed(&newtio,B4800);
            break;
        case 9600:
            cfsetispeed(&newtio,B9600);
            cfsetospeed(&newtio,B9600);
            break;
        case 19200:
            cfsetispeed(&newtio,B19200);
            cfsetospeed(&newtio,B19200);
            break;
        case 115200:
            cfsetispeed(&newtio,B115200);
            cfsetospeed(&newtio,B115200);
            break;
        case 460800:
            cfsetispeed(&newtio,B460800);
            cfsetospeed(&newtio,B460800);
            break;
        default:
            cfsetispeed(&newtio,B115200);
            cfsetospeed(&newtio,B115200);
            break;
    }
    /*set stop bit*/
        if(nStop==1)
            newtio.c_cflag &= ~CSTOPB;
        else if(nStop==2)
            newtio.c_cflag |= CSTOPB;
    /*set waittime and mincharsize*/
        newtio.c_cc[VTIME]=0;
        newtio.c_cc[VMIN]=0;
    /*process char unrece*/
        tcflush(fd,TCIFLUSH);
    /*action newsetinfo*/
        if((tcsetattr(fd,TCSANOW,&newtio))!=0)
        {
            perror("com set error");
            return -1;
        }
        printf("set done!\n");
        return 0;
}

RcodeModule::RcodeModule()
{
    m_endTimerMs = 0;
    countdown_ms(0);
    m_rcodeDatas.clear();
    m_fd = open("/dev/ttyAMA3", O_RDWR);
    qt_debug() << "RcodeModule" << m_fd;
    int flags = fcntl(m_fd,F_GETFL,0);
    flags &= ~O_NONBLOCK;
    fcntl(m_fd,F_SETFL,flags);
    if(setOpt(m_fd,9600,8,'N',1) < 0)
    {
        perror("set_opt error");
        return;
    }
    tcflush(m_fd, TCIOFLUSH);
}

void RcodeModule::run()
{
    int userId = 0;
    QString isSuccess = "0";
    QString isStranger = "0";
    QByteArray rcodeDatas;
    rcodeDatas.clear();
    while(true)
    {
        int len, fs_sel;
        fd_set fs_read;
        struct timeval time;
        time.tv_sec = 0;              //set the rcv wait time
        time.tv_usec = 1000000;    //100000us = 0.1s
        FD_ZERO(&fs_read);        //每次循环都要清空集合，否则不能检测描述符变化
        FD_SET(m_fd, &fs_read);    //添加描述符
        fs_sel = select(m_fd + 1, &fs_read, nullptr, nullptr, &time);
        if(fs_sel)
        {
            char buf[8];
            len = read(m_fd, buf, 8);
            rcodeDatas.append(buf);
        }
        else
        {
            if (rcodeDatas.isEmpty() || switchCtl->m_rcode != 2)
            {
                continue ;
            }
            tcflush(m_fd, TCIOFLUSH);
            QByteArray content = QByteArray::fromBase64(rcodeDatas);
            QByteArray datas = content.mid(0, 65);
            QByteArrayList contentList = datas.split(',');
            qDebug() << contentList;
            bool ok;
            QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
            QDateTime current_time = QDateTime::currentDateTime().addSecs(28800);//显示时间，格式为：年-月-日 时：分：秒 周几
            qint64 nowTime = origin_time.secsTo(current_time);
            int starTime = contentList.mid(3, 4).join().toInt(&ok, 16);
            int endTime = contentList.mid(7, 4).join().toInt(&ok, 16) * 60 + starTime;
            ushort crc = contentList.mid(20, 2).join().toUShort(&ok, 16);
            ushort crc16 = CRC16_CCITT(datas);
            int type = contentList.at(0).toInt(&ok, 16);
            qt_debug() << type << crc << crc16 << starTime << nowTime << endTime;
            if(0xF8 == type)
            {
                if (crc == crc16)
                {
                    if(nowTime < endTime && nowTime > starTime)
                    {
                        if (contentList.at(11).toInt(&ok, 16))
                        {
                            int code = HttpsClient::httpsQRCode(QString(datas));
                            if (0 == code)
                            {
                                isStranger = "0";
                                isSuccess = "1";
                                emit rcodeResultShow(1, tr("您好"), tr("您好"));
                                hardware->playSound(tr("认证通过").toUtf8(), "chengong.aac");
                                hardware->ctlLed(GREEN);
                                hardware->checkOpenDoor();
                            }
                            else
                            {
                                isStranger = "1";
                                isSuccess = "0";
                                emit rcodeResultShow(1, tr("未注册"), tr("请联系管理员"));
                                hardware->playSound(tr("未注册").toUtf8(), "shibai.aac");
                                hardware->ctlLed(RED);
                            }
                        }
                        else
                        {
                            int iphone = contentList.mid(12, 4).join().toInt(&ok, 16);
                            qt_debug() << iphone;
                            userId = sqlDatabase->sqlSelectMobile(QString("%1").arg(iphone));
                            if(userId > 1)
                            {
                                isStranger = "0";
                                isSuccess = "1";
                                emit rcodeResultShow(1, tr("您好"), tr("您好"));
                                hardware->playSound(tr("认证通过").toUtf8(), "chengong.aac");
                                hardware->ctlLed(GREEN);
                                hardware->checkOpenDoor();
                            }
                            else {
                                isStranger = "1";
                                isSuccess = "0";
                                emit rcodeResultShow(1, tr("未注册"), tr("请联系管理员"));
                                hardware->playSound(tr("未注册").toUtf8(), "shibai.aac");
                                hardware->ctlLed(RED);
                            }
                        }
                    }
                    else {
                        isStranger = "0";
                        isSuccess = "0";
                        emit rcodeResultShow(1, tr("已过期"), tr("已过期"));
                        hardware->playSound(tr("未授权").toUtf8(), "authority.aac");
                        hardware->ctlLed(RED);
                    }
                    QStringList datas;
                    datas.clear();
                    int offlineNmae = QDateTime::currentDateTime().toTime_t();
                    datas << QDateTime::currentDateTime().addSecs(28800).toString("yyyy-MM-dd HH:mm:ss") << "" << isSuccess << "" << isStranger
                          << "" << "" << "" << "" << "" << "";
                    emit uploadopenlog(offlineNmae, userId, "", 0, 6, 1, 0, datas);
                    sqlDatabase->sqlInsertOffline(offlineNmae, userId, 6, 0, 0, 0, datas);
                }
                else {
                    emit rcodeResultShow(1, tr("无效二维码"), tr("无效二维码"));
                    hardware->playSound(tr("未注册").toUtf8(), "shibai.aac");
                    hardware->ctlLed(RED);
                }
            }
            else if(0xF1 == type || 0xF2 == type)
            {
                if (crc == crc16 )
                {
                    if(nowTime < endTime && nowTime > starTime)
                    {
                        userId = contentList.mid(12, 4).join().toInt(&ok, 16);
                        QVariantList users = sqlDatabase->sqlSelect(userId);
                        if(users.size() > 0)
                        {
                            isStranger = "0";
                            isSuccess = "1";
                            emit rcodeResultShow(1, tr("您好"), tr("您好"));
                            hardware->playSound(tr("认证通过").toUtf8(), "chengong.aac");
                            hardware->ctlLed(GREEN);
                            hardware->checkOpenDoor();
                        }
                        else {
                            isStranger = "1";
                            isSuccess = "0";
                            emit rcodeResultShow(1, tr("未注册"), tr("请联系管理员"));
                            hardware->playSound(tr("未注册").toUtf8(), "shibai.aac");
                            hardware->ctlLed(RED);
                        }
                    }
                    else {
                        isStranger = "0";
                        isSuccess = "0";
                        emit rcodeResultShow(1, tr("已过期"), tr("已过期"));
                        hardware->playSound(tr("未授权").toUtf8(), "authority.aac");
                        hardware->ctlLed(RED);
                    }
                    QStringList datas;
                    datas.clear();
                    int offlineNmae = QDateTime::currentDateTime().toTime_t();
                    datas << QDateTime::currentDateTime().addSecs(28800).toString("yyyy-MM-dd HH:mm:ss") << "" << isSuccess << "" << isStranger
                          << "" << "" << "" << "" << "" << "";
                    emit uploadopenlog(offlineNmae, userId, "", 0, 6, 1, 0, datas);
                    sqlDatabase->sqlInsertOffline(offlineNmae, userId, 6, 0, 0, 0, datas);
                }
                else
                {
                    emit rcodeResultShow(1, tr("无效二维码"), tr("无效二维码"));
                    hardware->playSound(tr("未注册").toUtf8(), "shibai.aac");
                    hardware->ctlLed(RED);
                }
            }
            else
            {
                emit rcodeResultShow(1, tr("无效二维码"), tr("无效二维码"));
                hardware->playSound(tr("未注册").toUtf8(), "shibai.aac");
                hardware->ctlLed(RED);
            }
            content.clear();
            contentList.clear();
            rcodeDatas.clear();
            sleep(2);
        }
    }
}

void RcodeModule::recvRcodeResult(const QByteArray &results)
{
    if(expired())
    {
        int userId = 0;
        QString isSuccess = "0";
        QString isStranger = "0";
        m_rcodeDatas = results;
        QByteArrayList contentList = m_rcodeDatas.split(',');
        bool ok;
        QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
        QDateTime current_time = QDateTime::currentDateTime();//显示时间，格式为：年-月-日 时：分：秒 周几
        qint64 nowTime = origin_time.secsTo(current_time);
        int starTime = contentList.mid(3, 4).join().toInt(&ok, 16);
        int endTime = contentList.mid(7, 4).join().toInt(&ok, 16) * 60 + starTime;
        ushort crc = contentList.mid(20, 2).join().toUShort(&ok, 16);
        ushort crc16 = CRC16_CCITT(m_rcodeDatas);
        int type = contentList.at(0).toInt(&ok, 16);
        qt_debug() << type << crc << crc16 << starTime << nowTime << endTime;
        if(0xF1 == type || 0xF2 == type)
        {
            if (crc == crc16 )
            {
                if(nowTime < endTime && nowTime > starTime)
                {
                    userId = contentList.mid(12, 4).join().toInt(&ok, 16);
                    QVariantList users = sqlDatabase->sqlSelect(userId);
                    if(users.size() > 0)
                    {
                        isStranger = "0";
                        isSuccess = "1";
                        emit rcodeResultShow(1, tr("您好"), tr("您好"));
                        hardware->playSound(tr("认证通过").toUtf8(), "chengong.aac");
                        hardware->ctlLed(GREEN);
                        hardware->checkOpenDoor();
                    }
                    else {
                        isStranger = "1";
                        isSuccess = "0";
                        emit rcodeResultShow(1, tr("未注册"), tr("未注册"));
                        hardware->playSound(tr("未注册").toUtf8(), "authority.aac");
                        hardware->ctlLed(RED);
                    }
                }
                else {
                    isStranger = "0";
                    isSuccess = "0";
                    emit rcodeResultShow(1, tr("已过期"), tr("已过期"));
                    hardware->playSound(tr("未授权").toUtf8(), "authority.aac");
                    hardware->ctlLed(RED);
                }
                QStringList datas;
                datas.clear();
                int offlineNmae = QDateTime::currentDateTime().toTime_t();
                datas << QDateTime::currentDateTime().addSecs(28800).toString("yyyy-MM-dd HH:mm:ss") << "" << isSuccess << "" << isStranger
                      << "" << "" << "" << "" << "" << "";
                emit uploadopenlog(offlineNmae, userId, "", 0, 6, 1, 0, datas);
                sqlDatabase->sqlInsertOffline(offlineNmae, userId, 6, 0, 0, 0, datas);
            }
            else
            {
                emit rcodeResultShow(1, tr("无效二维码"), tr("无效二维码"));
                hardware->playSound(tr("未注册").toUtf8(), "shibai.aac");
                hardware->ctlLed(RED);
            }
        }
        else
        {
            emit rcodeResultShow(1, tr("无效二维码"), tr("无效二维码"));
            hardware->playSound(tr("未注册").toUtf8(), "shibai.aac");
            hardware->ctlLed(RED);
        }
        contentList.clear();
        m_rcodeDatas.clear();
        countdown_ms(2*1000);
    }
}

bool RcodeModule::expired()
{
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time =QDateTime::currentDateTime();
    qint64 now = origin_time.msecsTo(current_date_time);
    if(now >= m_endTimerMs)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void RcodeModule::countdown_ms(int ms)
{
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time =QDateTime::currentDateTime();
    qint64 now = origin_time.msecsTo(current_date_time);
    m_endTimerMs = now + ms;
}
