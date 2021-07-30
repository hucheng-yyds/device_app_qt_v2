#include <fcntl.h>
#include "wgmodule.h"

WgModule::WgModule()
{

}

void WgModule::run()
{
    m_fd = open("/dev/wgSR", O_RDWR);
    qt_debug() << "open========" << m_fd;
    if (-1 == m_fd)
    {
        return ;
    }
    weigen_d wgd;
    while(true)
    {
        wgd.buf[0] = WG_DATA_R;
        wgd.buf[1] = 0;
        wgd.buf[2] = 0;
        wgd.buf[3] = 0;
        wgd.buf[4] = 0;
        wgd.buf[5] = 0;
        ioctl(m_fd, WG_DATA_WR, &wgd.buf);
        if(!wgd.buf[0] && wgd.buf[1] && wgExpired())
        {
            QByteArray data(wgd.buf[1], 0);
            data[0] = wgd.buf[2];
            data[1] = wgd.buf[3];
            data[2] = wgd.buf[4];
            if (4 == wgd.buf[1])
            {
                data[3] = wgd.buf[5];
            }
            data = data.toHex();
            QString cardNo = QString::fromUtf8(data).mid(0, 8).toLower();
            int time = 3*1000;
            if(switchCtl->m_tempCtl)
            {
                time = 4*1000;
            }
            int mid = sqlDatabase->sqlSelectIcId(cardNo);
            wgCountdown_ms(time);
            emit sigWgInfo(mid, cardNo);
        }
        msleep(200);
    }
}

void WgModule::wgOut(QByteArray number)
{
    if (number.isEmpty() || number.size() > 8)
    {
        qt_debug() << "wgOut error !";
        return ;
    }
    QByteArray array = QByteArray::fromHex(number);
    weigen_d wgd;
    wgd.buf[0] = WG_DATA_W;
    wgd.buf[1] = array.size();
    wgd.buf[2] = array[0];
    wgd.buf[3] = array[1];
    wgd.buf[4] = array[2];
    wgd.buf[5] = array[3];
    ioctl(m_fd, WG_DATA_WR, &wgd.buf);
    qt_debug() << "wg out" << number;
}

void WgModule::wgOutuChar(int type, char *buf)
{
    if (!buf)
    {
        qt_debug() << "wgOutuChar is Null !";
        return ;
    }
    weigen_d wgd;
    wgd.buf[0] = WG_DATA_W;
    wgd.buf[1] = type;
    wgd.buf[2] = buf[2];
    wgd.buf[3] = buf[3];
    wgd.buf[4] = buf[4];
    wgd.buf[5] = buf[5];
    qt_debug() << "WG_DATA_WR out" << wgd.buf[0]<<wgd.buf[1]<<wgd.buf[2]<<wgd.buf[3]<<wgd.buf[4]<<wgd.buf[5];
    ioctl(m_fd, WG_DATA_WR, &wgd.buf);
}

bool WgModule::wgExpired()
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

void WgModule::wgCountdown_ms(int ms)
{
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time =QDateTime::currentDateTime();
    qint64 now = origin_time.msecsTo(current_date_time);
    m_endTimerMs = now + ms;
}
