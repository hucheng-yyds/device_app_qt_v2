#include <QNetworkInterface>
#include "netmanager.h"

NetManager::NetManager()
{

}

void NetManager::run()
{
    QString ip = getIP();
    int seq = 0;
    while(true)
    {
        if(seq > 2)
        {
            ip = getIP();
            seq = 0;
        }
        int count = sqlDatabase->m_localFaceSet.size();
        emit showDeviceInfo(VERSION, switchCtl->m_devName, QString("%1").arg(count), ip, "");
        msleep(500);
        seq++;
    }
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
                if(interface.name() == "eth0")
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
            }
        }
    }
    return ipAddr;
}
