#include <QNetworkInterface>
#include <QDateTime>
#include <QTcpSocket>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/mii.h>
#include <linux/sockios.h>
#include <errno.h>
#include "netmanager.h"
#include "datashare.h"

int get_if_miireg(const char *if_name, int reg_num )
{
    int fd = -1;
    struct ifreq ifr;
    struct mii_ioctl_data *mii;
    int value;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        close(fd);
        return -1;
    }

    bzero(&ifr, sizeof(ifr));
    strncpy(ifr.ifr_name, if_name, IFNAMSIZ-1);
    ifr.ifr_name[IFNAMSIZ-1] = 0;

    if (ioctl(fd, SIOCGMIIPHY, &ifr) < 0)
    {
        perror("ioctl");
        close(fd);
        return -1;
    }

    mii = (struct mii_ioctl_data *)&ifr.ifr_data;
    mii->reg_num = reg_num;//0x01
    if (ioctl(fd, SIOCGMIIREG, &ifr) < 0)
    {
        perror("ioctl");
        close(fd);
        return -1;
    }
    close(fd);
    value = ((mii->val_out&0x04)>>2);
    return value;
}

void getNtp()
{
    qt_debug() <<"connectToHosts--------------";
    QTcpSocket *socket = new QTcpSocket();
    socket->connectToHost("time.nist.gov", 13);
    qt_debug() <<"--------------connectToHosts";
    if (socket->waitForConnected())
    {   if (socket->waitForReadyRead())
        {
            QString str(socket->readAll());
            str = str.trimmed();
            str = str.section(" ", 1, 2);
            str = "20"+str;
            system("date -s '"+str.toLatin1()+"'");
        }
    }
    socket->close();
    delete socket;
}

NetManager::NetManager()
{
    m_fourG = false;
    m_wifi = false;
    m_wpa = new WpaGui;
    connect(m_wpa, &WpaGui::connected, this, &NetManager::onConnected);
    connect(m_wpa, &WpaGui::disconnected, this, &NetManager::onDisconnected);
    connect(m_wpa, &WpaGui::pskError, this, &NetManager::onPskError);
}

void NetManager::run()
{
    QString ip = getIP();
    int netWorkMode = 7;
    int seq = 0;
    int size = 0;
    int netStatus = false;
    while(true)
    {
        m_eth0 = get_if_miireg("eth0", 0x01);
        if(seq > 2)
        {
            ip = getIP();
            seq = 0;
            if(m_eth0 <= 0)
            {
                QString userName = switchCtl->m_wifiName;
                QString userPwd = switchCtl->m_wifiPwd;
                int value = m_wpa->updateStatus();
                netWorkMode = 0;
//                if(value > 80)
//                {
//                    netWorkMode = 0;
//                }
//                else if(value > 60)
//                {
//                    netWorkMode = 1;
//                }
//                else if(value > 20){
//                    netWorkMode = 2;
//                }
//                else {
//                    dataShare->m_netStatus = false;
//                    m_wifi = false;
//                    if (WpaGui::TrayIconConnected == m_wpa->state())
//                    {
//                        m_wpa->removeNetwork();
//                    }
//                    if(WpaGui::TrayIconOffline != m_wpa->state())
//                    {
//                        m_wpa->setState(WpaGui::TrayIconOffline);
//                        m_wpa->removeNetwork();
//                    }
//                }
                size++;
                if(size > 60)
                {
                    size = 0;
                    qt_debug() << "===============" << value << userName << userPwd << m_wpa->state();
                }
                if (!userName.isEmpty() && !userPwd.isEmpty() && WpaGui::TrayIconOffline == m_wpa->state())
                {
                    m_wpa->enableNetwork(userName.toUtf8().data(), userPwd.toUtf8().data(), AUTH_WPA2_PSK);
                }
            }
            else {
                m_wifi = false;
                if (WpaGui::TrayIconConnected == m_wpa->state())
                {
                    m_wpa->removeNetwork();
                }
                if(WpaGui::TrayIconOffline != m_wpa->state())
                {
                    m_wpa->setState(WpaGui::TrayIconOffline);
                    m_wpa->removeNetwork();
                }
            }
        }
        if(m_eth0 > 0)
        {
            emit networkChanged(6, dataShare->m_netStatus);
        }
        else if(m_wifi)
        {
            emit networkChanged(netWorkMode, dataShare->m_netStatus);
        }
        else
        {
            dataShare->m_netStatus = false;
            emit networkChanged(7, false);
        }
        if (netStatus != dataShare->m_netStatus &&
                dataShare->m_netStatus) {
            getNtp();
        }
        netStatus = dataShare->m_netStatus;
        dataShare->m_ipAddr = ip;
        int count = sqlDatabase->m_localFaceSet.size();
        emit showDeviceInfo(switchCtl->m_tempCtl, switchCtl->m_faceDoorCtl, VERSION, switchCtl->m_devName, QString("%1").arg(count), ip, switchCtl->m_sn);
        msleep(100);
        int second = getTimeZoneMin()*60 + getTimeZone()*3600;
        QDateTime dateTime = QDateTime::currentDateTime().addSecs(second);
        QString curDate = getCurrentTime(dateTime);
        int hour = dateTime.toString("HH").toInt();
        int min = dateTime.toString("mm").toInt();
        QString date = curDate.split(" ").at(1);
        emit timeSync(curDate, dateTime.toString("HH:mm"), hour, min, date + " " + dateTime.toString("yy.MM.dd"));
        msleep(500);
        seq++;
    }
}

void NetManager::onConnected()
{
    m_wifi = true;
    qt_debug() << "wifi connect success";
}

void NetManager::onDisconnected()
{
    m_wifi = false;
    qt_debug() << "wifi disconnect";
}

void NetManager::onPskError()
{
    qt_debug() << "onPskError";
}

QString NetManager::getCurrentTime(QDateTime dataTime)
{
    QLocale locale;
    if(0 == switchCtl->m_language)
    {
        locale = QLocale::Chinese;//指定中文显示
    }
    else if(1 == switchCtl->m_language)
    {
        locale = QLocale::English;//指定英文显示
    }
    QString time = QString(locale.toString(dataTime, QString("MM-dd dddd")));
    return time;
}

int NetManager::getTimeZone()
{
    int hourNum = 0;
    QString timeZoneStr = switchCtl->m_timeZone;
    if(timeZoneStr.contains("(UTC"))
    {
        QString hour = timeZoneStr.right(7).mid(1,5);
        if(timeZoneStr.right(7).at(0) == "-")
        {
           hourNum = 0 - hour.mid(0,2).toInt();
        }
        else if(timeZoneStr.right(7).at(0) == "+")
        {
            hourNum = hour.mid(0,2).toInt();
        }
//        qt_debug() << hour << hourNum;
    }
    return hourNum;
}

int NetManager::getTimeZoneMin()
{
    int minuteNum = 0;
    QString timeZoneStr = switchCtl->m_timeZone;
    if(timeZoneStr.contains("(UTC"))
    {
        QString timeStr = timeZoneStr.right(7).mid(1,5);
        if(timeZoneStr.right(7).at(0) == "-")
        {
           minuteNum = 0 - timeStr.mid(3,2).toInt();
        }
        else if(timeZoneStr.right(7).at(0) == "+")
        {
            minuteNum = timeStr.mid(3,2).toInt();
        }
//        qt_debug() << hour << hourNum;
    }
    return minuteNum;
}

QString NetManager::getIP()
{
    QString ipAddr = QString("0.0.0.0");
    QList<QNetworkInterface>list=QNetworkInterface::allInterfaces();//获取所有网络接口信息
    foreach(QNetworkInterface interface,list)
    {
        QList<QNetworkAddressEntry> addressEntryList = interface.addressEntries();
        foreach(QNetworkAddressEntry addressEntryItem, addressEntryList)
        {
            if(addressEntryItem.ip().protocol()==QAbstractSocket::IPv4Protocol)
            {
                if(m_eth0 > 0 && interface.name() == "eth0")
                {
                    bool manual = false;
                    if(!switchCtl->m_ipMode)
                    {
                        ipAddr = switchCtl->m_manualIp;
                        QString netmask = switchCtl->m_manualNetmask;
                        QString gateway = switchCtl->m_manualGateway;
                        QString dns = switchCtl->m_manualDns;
                        if(ipAddr != "")
                        {
                            QStringList ips = ipAddr.split(".");
                            int status = true;
                            if(4 == ips.size())
                            {
                                for(int i = 0;i < ips.size();i++)
                                {
                                    int ch = ips.at(i).toInt();
                                    if(ch <= 1 || ch >= 255)
                                    {
                                        status = false;
                                    }
                                }
                            }
                            if(status)
                            {
                                manual = true;
                                QString ipCmd = QString("ifconfig eth0 %1 netmask %2").arg(ipAddr).arg(netmask);
                                QString gatewayCmd = QString("route add default gw %1 dev eth0").arg(gateway);
                                QString dnsCmd = QString("echo nameserver %1 >> /etc/resolv.conf").arg(dns);
                                system(ipCmd.toLatin1().data());
                                system(gatewayCmd.toLatin1().data());
                                system(dnsCmd.toLatin1().data());
                            }
                        }
                    }
                    if(!manual)
                    {
                        QList<QNetworkAddressEntry>entryList=interface.addressEntries();
                        ipAddr = entryList.value(0).ip().toString();
                        addressEntryList.clear();
                    }
                    break;
                }
                else if(m_wifi && interface.name() == "wlan0")
                {
                    QList<QNetworkAddressEntry>entryList=interface.addressEntries();
                    ipAddr = entryList.value(0).ip().toString();
                    break;
                }
            }
        }
    }
    return ipAddr;
}
