#include "tempmanager.h"
#include <string>
#include <vector>
#include <sys/stat.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <QDataStream>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include "datashare.h"

TempManager::TempManager()
{
    moveToThread(this);
}

void TempManager::run()
{
    m_recTempDataOK = false;
    checkUART();
    m_timer = new QTimer;
    m_timer->setInterval(200);
    connect(m_timer, &QTimer::timeout, this, &TempManager::timeckeck);
    m_timer->start();
    m_tempStatus = false;
    m_startTemp = false;
    exec();
}

static void hexdump(const char *notice, void *buffer, int len)
{
    int i=0;
    unsigned char *ptr=(unsigned char *)buffer;
    printf("[%s][lentgh=%d]",notice,len);
    for(i = 0; i < len; i ++)
    {
        printf("0x%.2X ",*(ptr+i));
//        data[i + c*8] = *(ptr+i);
    }
    printf("\n");
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

bool TempManager::checkCRC(QVector<uchar> buff)
{
    int sum = 0;
    int bufSum = ((buff[1545] << 8) + buff[1544]) & 0xffff;
    for (int i = 0; i < 1544; i += 2) {
        sum += buff[i] + (buff[i + 1] << 8);
    }
    sum &= 0xffff;
//    qt_debug() << sum << bufSum;
    return sum == bufSum;
}

void TempManager::checkUART()
{
    m_fd = open("/dev/ttyAMA1", O_RDWR);
    qt_debug() << "fd1: " << m_fd;
    if(setOpt(m_fd, 115200, 8, 'N', 1) < 0)
    {
        perror("set_opt error");
    }
}

// 算法初始化完毕后触发 用温度值是否为0判断测温模块是否存在
float TempManager::onIsTemp()
{
    getTemperature();
    return m_tempModule;
}

// DE22关全屏测温，DF21
void TempManager::openAllScreenTemp(bool status)
{
    bool flag = false;
    int join_len = 0;
    QVector<uchar> join_buf;
    unsigned char buf[2] = {uchar(251), uchar(5)};
    tcflush(m_fd, TCIOFLUSH);
    if(status)
    {
        buf[0] = uchar(223);
        buf[1] = uchar(33);
    }
    else {
        buf[0] = uchar(222);
        buf[1] = uchar(34);
    }
    int len = write(m_fd, buf, 2);
    if (len > 0) {
        qt_debug() << "serial write" << buf[0] <<  buf[1] << len;
    }
    msleep(200);
    join_buf.clear();
    join_len = 0;
    while (true) {
        uchar buf[4095];
        int len = read(m_fd, buf, 4095);
        if (!len) {
            break;
        }
        join_len += len;
        for (int i = 0; i < len; i ++) {
            join_buf << buf[i];
        }
        if(join_len > 10000)
        {
            break;
        }
    }
    printf("read len:%d\n", join_len);
    for(int i = 0;i < join_len;i++)
    {
        printf("%#x ", join_buf[i]);
    }
    printf("\n\n");
    unsigned char sum = 0;
    for(int i = join_len-9;i >= 0;i--)
    {
        if(0x5a == join_buf[i])
        {
            for(int j = 0;j < 8;j++)
            {
                sum += join_buf[i+j];
            }
            sum = 256 - sum;
            qDebug() << sum << join_buf[i+8];
            if(join_buf[i+8] == sum)
            {
                flag = true;
            }
        }
    }
    if(!flag)
    {
        msleep(500);
        openAllScreenTemp(status);
    }
}

void TempManager::startTemp()
{
    if(expired())
    {
        int lens = 0;
        while (true) {
            uchar buf[4095];
            int len = read(m_fd, buf, 4095);
            if (len <= 0)
            {
                if(len < 0)
                {
                    close(m_fd);
                    checkUART();
                }
                qt_debug() << "get temp data end" << len << m_fd;
                break;
            }
            lens += len;
            if(lens > 20000)
            {
                qt_debug() << "start temp size > 20000";
                break;
            }
        }
        int count = 1500;
        if(switchCtl->m_loose)
        {
            count = 500;
        }
        countdown_ms(count);
        m_tempStatus = false;
        m_startTemp = true;
        tcflush(m_fd, TCIOFLUSH);
    }
}

void TempManager::tcflsh()
{
    tcflush(m_fd, TCIOFLUSH);
}

void TempManager::timeckeck()
{
    if(m_startTemp)
    {
        if(expired())
        {

            m_tempCount = 0;
            QString tempVal = getTemperature();
            int result = compareTemp(tempVal);
            m_tempCallBack->setTempResult(tempVal, result);
//            dataShare->m_tempFlag = true;
//            dataShare->m_tempVal = tempVal;
//            dataShare->m_tempResult = result;
            m_startTemp = false;
        }
    }
}

bool TempManager::expired()
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

void TempManager::countdown_ms(int ms)
{
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time =QDateTime::currentDateTime();
    qint64 now = origin_time.msecsTo(current_date_time);
    m_endTimerMs = now + ms;
}

QString TempManager::getTemperature()
{
    float value = 0;
    float maxVal = 0;
    int join_len = 0;
    QString tempVal = "0";
    QVector<uchar> join_buf;
    join_buf.clear();
    qt_debug() << "start get temp";
    while (true) {
        uchar buf[4095];
        memset(buf, 0, 4095);
        int len = read(m_fd, buf, 4095);
        if (len <= 0)
        {
            if(len < 0)
            {
                close(m_fd);
                checkUART();
            }
            qt_debug() << "get temp data end" << len << m_fd;
            break;
        }
        join_len += len;
        for (int i = 0; i < len; i ++) {
            join_buf << buf[i];
        }
        if(join_len > 20000)
        {
            qt_debug() << "get temp data size > 10000";
            break;
        }
    }
    if(join_len < 1546 && join_len > 0)
    {
        return "0.0";
    }
    for (int i = join_len-1546; i >= 0; i --)
    {
        if (0x5A == join_buf[i] &&
                0x5A == join_buf[i + 1] &&
                0x04 == join_buf[i + 2] &&
                0x06 == join_buf[i + 3])
        {
            value = (join_buf[i+1543] * 256 + join_buf[i+1542]) / 100.00;
            if (maxVal < value && checkCRC(join_buf.mid(i, 1546)))
            {
                maxVal = value;
                if(!switchCtl->m_loose)
                {
                    break;
                }
            }
        }
    }
    m_tempModule = maxVal;
    if(switchCtl->m_loose)
    {
        if (maxVal <= 36.0)
        {
            int m_num = 0;
            m_num = (int)(maxVal*100.00);
            m_num = m_num%3;
            maxVal = (float)(m_num)/10.0;
            maxVal += 36.0;
        }
    }
    else
    {
        if ((maxVal <= 36.0) && (maxVal >= 28.0))
        {
            int m_num = 0;
            m_num = (int)(maxVal*100.00);
            m_num = m_num%3;
            maxVal = (float)(m_num)/10.0;
            maxVal += 36.0;
        }
    }
    maxVal += switchCtl->m_tempComp;
    if(switchCtl->m_fahrenheit)
    {
        maxVal = (maxVal)*1.8 + 32.0;
    }
    tempVal = QString::number(maxVal, 'f', 1);
    if(m_tempStatus && maxVal < 36.0)
    {
        tempVal = "0";
    }
    qt_debug() << "end get temp" << tempVal;
    return tempVal;
}

void TempManager::endTemp()
{
    m_tempStatus = true;
}

int TempManager::compareTemp(const QString &tempVal)
{
    int result = 0;
    float normalTemp = 0.0;
    float warnValue = switchCtl->m_warnValue;
    if(switchCtl->m_fahrenheit)
    {
        normalTemp = 35.0*1.8 + 32.0;
        warnValue = (warnValue)*1.8 + 32.0;
    }
    else
    {
        normalTemp = 28.0;
    }
    if(tempVal.toFloat() >= warnValue)
    {
        result = 0;
    }
    else if(tempVal.toFloat() > normalTemp)
    {
        result = 1;
    }
    else if(tempVal.toFloat() >= 1.0)
    {
        result = -1;
    }
    else {
        result = -2;
    }
    if(m_tempStatus && tempVal < "36.0")
    {
        result = -2;
    }
    qt_debug() << "temp check end" << result << tempVal;
    return result;

}

void TempManager::onRecvStopTemp()
{
    m_recTempDataOK = true;
}

void TempManager::onSendCmdToTemp(QByteArray tempInfo)
{
    qt_debug() << tempInfo;
    m_recTempDataOK = false;
    if(tempInfo.count() > 0)
    {
        bool ok = false;
        int a = tempInfo.mid(0,2).toInt(&ok, 16);
        int b = tempInfo.mid(2,2).toInt(&ok, 16);
        uchar send[2] = {uchar(a),uchar(b)};

        int len = write(m_fd, send, sizeof(send));
        if (len > 0) {
            hexdump("serial write",send,len);
        }
        QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
        QDateTime current_time = QDateTime::currentDateTime();//显示时间，格式为：年-月-日 时：分：秒 周几
        m_timeTemp = origin_time.secsTo(current_time);
    }
    do{
        getTempReturn();
        msleep(1);
    }while(!m_recTempDataOK);
}

void TempManager::getTempReturn()
{
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_time = QDateTime::currentDateTime();//显示时间，格式为：年-月-日 时：分：秒 周几
    qint64 nSeconds = origin_time.secsTo(current_time);
    if(nSeconds - m_timeTemp > 5)
    {
        m_recTempDataOK = true;
        emit tempeatureInfo("0");
        return;
    }
    int join_len = 0;
    QVector<uchar> join_buf;
    uchar buf[4095];
    int len = read(m_fd, buf, 4095);
    if (!len)
    {
        return;
    }
    join_len += len;
    for (int i = 0; i < len; i ++)
    {
        join_buf << buf[i];
    }
    for (int i = join_len; i >= 0; i --)
    {
        if((0x5A == join_buf[i])||(0x5a == join_buf[i]))
        {
            QByteArray tempInfo;
            for(int t=0; t<9 ; t++)
            {
                tempInfo.append(join_buf[t]);
            }
            if((tempInfo.at(0) == 0x5a)&&(tempInfo.at(1) == 0x02))
            {
                emit tempeatureInfo(tempInfo);
                m_recTempDataOK = true;
                break;
            }
            emit tempeatureInfo(tempInfo);
            break;
        }
    }
}

void TempManager::getTempInfo()
{
    int join_len = 0;
    QVector<uchar> join_buf;
    tcflush(m_fd, TCIOFLUSH);
    unsigned char buf1[2] = {uchar(251), uchar(5)};
    int len1 = write(m_fd, buf1, 2);
    msleep(200);
    join_buf.clear();
    join_len = 0;
    while (true)
    {
        uchar buf[4095];
        int len = read(m_fd, buf, 4095);
        if (!len)
        {
            break;
        }
        join_len += len;
        for (int i = 0; i < len; i ++)
        {
            join_buf << buf[i];
        }
        if(join_len > 10000)
        {
            break;
        }
    }
    unsigned char sum = 0;
    for(int i = join_len-9;i >= 0;i--)
    {
        if(0x5a == join_buf[i])
        {
            for(int j = 0;j < 8;j++)
            {
                sum += join_buf[i+j];
            }
            sum = 256 - sum;
            qt_debug() << sum << join_buf[i+8];
            if(join_buf[i+8] == sum)
            {
                dataShare->m_offset = join_buf[i+2];
                dataShare->m_tempType = QString("%1.%2").arg(join_buf[i+3]).arg(join_buf[i+4]);
                dataShare->m_tempVer = QString("%1").arg(join_buf[i+6]);
                dataShare->m_offdata = join_buf[i+5];
                dataShare->m_tempDevice = QString("%1").arg(join_buf[i+7]);
            }
        }
    }
}


// 以下为测温模块ota升级部分
static unsigned short CRC16(unsigned long CRC_Sumx,unsigned char *CRC_Buf, unsigned short CRC_Leni)
 {
  unsigned long i, j;
  for (i = 0; i < CRC_Leni; i++) {
    CRC_Sumx ^= *(CRC_Buf + i);
    for (j = 0; j < 8; j++) {
      if (CRC_Sumx & 0x01) {
        CRC_Sumx >>= 1;
        CRC_Sumx ^= 0xA001;
      } else {
        CRC_Sumx >>= 1;
      }
    }
  }
  return (CRC_Sumx);
}

int TempManager::update_write(update_protocol_t *This,unsigned char *dat,unsigned short length,unsigned char *out)
{
    This->len[0] = 0xff&(length>>8);
    This->len[1] = 0xff&(length);
    memset(out, 0xff, 4);
    memcpy(out+4, This->addr, 4);
    memcpy(out+8, This->len, 2);
    memcpy(out+10, dat, length);
    unsigned short crc_c = CRC16(0, out, sizeof(update_protocol_t)+length);
    out[10+length] = 0xff&(crc_c>>8);
    out[10+length+1] = 0xff&(crc_c);
    return 10 + length + 2;
}

unsigned short get_num(update_protocol_t *This)
{
    unsigned short len=This->len[0];
    len=(len<<8)|This->len[1];
    return len;
}

int update_read(update_protocol_t *This,unsigned char *dat,int length,unsigned char *out)
{
    update_protocol_t *protocol_explain=(update_protocol_t *)dat;
    if(length<sizeof(update_protocol_t))
    {
        return -1;//长度错误
    }
    if(memcmp(This->heard,dat,sizeof(This->heard))!=0)
    {
        return -2;//头地址错误
    }
    This[0]=protocol_explain[0];
    unsigned short len=get_num(This);

    if(length<(len+sizeof(update_protocol_t)+2))
    {
        return 0;
    }
    unsigned short crc_c=CRC16(0,dat,len+sizeof(update_protocol_t));
    if(dat[len+sizeof(update_protocol_t)]!=(0xff&(crc_c>>8))||dat[len+sizeof(update_protocol_t)+1]!=(0xff&crc_c))
    {
        return -3; //校验值 不对
    }
     memcpy(out,dat+sizeof(update_protocol_t),len);
     return len;
}

void TempManager::openTempFile(const QString &name)
{
    QFile *file = new QFile;
    /*
    * 读取Bin文件
    */
    file->setFileName(name);
    if(file->open(QIODevice::ReadOnly)){
        m_upgradeFileSize = 0;
        QDataStream BinFileData(file);
        m_fileSize = file->size();
        qt_debug() << "filesize:" << m_fileSize;
        m_fileBuf = new char[m_fileSize];
        BinFileData.readRawData(m_fileBuf,static_cast<int>(m_fileSize));
        file->close();
    }
    else
    {
        qt_debug() << "file read fail!";
        return;
    }
}

void TempManager::sendTempProgram()
{
    unsigned char buf[2] = {uchar(250), uchar(6)};
    int len1 = write(m_fd, buf, 2);
    msleep(600);
    tcflush(m_fd, TCIOFLUSH);
    checkUART();
    msleep(500);
    openTempFile("temp.bin");
    update_protocol_t  pdemo[1];
    unsigned char out[1024+20];
    memset(pdemo->heard,0xff,sizeof(pdemo->heard));
    pdemo->addr[0]=0;
    pdemo->addr[1]=0;
    pdemo->addr[2]=0;
    pdemo->addr[3]=0;
    memset(out,0,sizeof(out));
    int ret=0;
    if(m_fileSize > 1024)
    {
        ret = update_write(pdemo, (unsigned char *)m_fileBuf, 1024, out);
        m_upgradeFileSize = 1024;
    }
    else
    {
        ret = update_write(pdemo, (unsigned char *)m_fileBuf, m_fileSize, out);
        m_upgradeFileSize = m_fileSize;
    }
    tcflush(m_fd, TCIOFLUSH);
    int len = write(m_fd, out, ret);
    if (len > 0)
    {
        printf("serial write %d\n",len);
    }
    do
    {
        recvUpdateData();
    }while(!sendOk);
}

void TempManager::recvUpdateData()
{
    int join_len = 0;
    bool status = false;
    uchar join_buf[4095];
    uchar buf[4095];
    memset(join_buf, 0, 4095);
    memset(buf, 0, 4095);
    int len = read(m_fd, buf, 4095);
    printf("%d %d\n",len, sendOk);
    if (!len)
    {
        return;
    }
    for (int i = 0; i < len; i++)
    {
        join_buf[i] = buf[i];
    }
    join_len += len;
    unsigned char out[1024+20];
    update_protocol_t  pdemo[1];
    memset(pdemo->heard, 0xff, sizeof(pdemo->heard));
    int ret=update_read(pdemo,join_buf,join_len,(unsigned char*)out);
    if(ret>0)
    {
        memset(join_buf, 0, 4096);
        join_len=0;
        pdemo->addr[0]=(m_upgradeFileSize>>8*3)&0xff;
        pdemo->addr[1]=(m_upgradeFileSize>>8*2)&0xff;
        pdemo->addr[2]=(m_upgradeFileSize>>8*1)&0xff;
        pdemo->addr[3]=(m_upgradeFileSize>>8*0)&0xff;
        int ch = m_fileSize - m_upgradeFileSize;
        int ret=0;
        if(ch>0)
        {
            if(ch>1024)
            {
                ret=update_write(pdemo,(unsigned char *)(m_fileBuf + m_upgradeFileSize),1024,out);
                m_upgradeFileSize+=1024;
            }
            else
            {
                ret=update_write(pdemo,(unsigned char *)(m_fileBuf + m_upgradeFileSize),ch,out);
                m_upgradeFileSize+=ch;
            }
            tcflush(m_fd, TCIOFLUSH);
            int len = write(m_fd, out, ret);
            if (len > 0)
            {
                printf("serial write %d\n",len);
            }
        }
        else
        {
            status = true;
        }
    }
    printf("======================================================fileAllSize:%d, %d\n", m_fileSize, m_upgradeFileSize);
    if(status && m_fileSize <= m_upgradeFileSize)
    {
        memset(pdemo->heard, 0xff, sizeof(pdemo->heard));
        pdemo->addr[0] = 0xff;
        pdemo->addr[1] = 0xff;
        pdemo->addr[2] = 0xff;
        pdemo->addr[3] = 0xff;
        unsigned char buf[2] = {'o', 'k'};
        ret=update_write(pdemo, buf, 2,out);
        int len = write(m_fd, out, ret);
        if (len > 0)
        {
            printf("serial write %d\n",len);
        }
        sendOk = true;
        system("rm temp.bin");
        delete m_fileBuf;
        m_fileBuf = nullptr;
        QByteArray data;
        data.append("tempok");
        emit tempeatureInfo(data);
    }
}
