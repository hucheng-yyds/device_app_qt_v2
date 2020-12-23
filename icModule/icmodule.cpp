#include "icmodule.h"
#include "datashare.h"

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

IcCardModule::IcCardModule()
{
    icCountdown_ms(0);
    m_icDatas.clear();
    m_fd = open("/dev/ttyS3", O_RDWR);
    qt_debug() << "IcCardModule" << m_fd;
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

void IcCardModule::run()
{
    QByteArray icDatas;
    icDatas.clear();
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
            icDatas.append(buf);
        }
        else
        {
            // 9AD69F18 \xD2\xCF\x0E;,\x01\x9A\xD6\x9F\x18WP
            if (icDatas.isEmpty())
            {
                msleep(200);
                continue ;
            }
            //d2cf0e3b2c019ad69fd618d65750
            //d2cf0e3b2c019ad69f189f579f509f
            if(icExpired())
            {
                QString cardNo = icDatas.toHex().mid(12, 8);
                qt_debug() << icDatas.toHex() << cardNo;
                int time = 3*1000;
                if(switchCtl->m_tempCtl)
                {
                    time = 4*1000;
                }
                int mid = sqlDatabase->sqlSelectIcId(cardNo);
                icCountdown_ms(time);
                emit sigIcInfo(mid, cardNo);
            }
            icDatas.clear();
        }
    }
}

bool IcCardModule::icExpired()
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

void IcCardModule::icCountdown_ms(int ms)
{
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time =QDateTime::currentDateTime();
    qint64 now = origin_time.msecsTo(current_date_time);
    m_endTimerMs = now + ms;
}
