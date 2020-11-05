#include "UdpServer.h"
#include <QJsonParseError>
#include <QJsonObject>
#include <QUuid>
#include <QDataStream>

typedef struct {
    QString version = VERSION;
    QString ip = "192.168.2.21";
    QString DevType;
    QString deviceSn;
}UdpInitDat;

static UdpInitDat initDat;

UdpServer::UdpServer()
{
    m_udpServer = new QUdpSocket;
//    m_udpServer->moveToThread(this);
      ///绑定，第一个参数为端口号，第二儿表示允许其它地址链接该广播
    m_udpServer->bind(13210, QUdpSocket::ShareAddress);
//    connect(m_udpServer, &QUdpSocket::readyRead, this, &UdpServer::processPendingDatagram);
//    sendData();
//    exec();
}

void UdpServer::run()
{
    while(true)
    {
        sendData();
        sleep(5);
    }
}

void UdpServer::sendData()
{
    QByteArray data;
    QByteArray sendDatas;
    QJsonDocument document;
    QJsonObject json;
    QDataStream out(&sendDatas, QIODevice::WriteOnly);
    QUuid uuid = QUuid::createUuid();
    QString strId = uuid.toString();
    json.insert("type",initDat.DevType);
    json.insert("version",initDat.version);
    json.insert("ip", initDat.ip);
    //json.insert("ip", "192.168.2.21");
    json.insert("uuid", strId);
    json.insert("mac", "");
    json.insert("ss_sn",initDat.deviceSn);
    document.setObject(json);
    data = document.toJson(QJsonDocument::Compact);
    sendDatas.append(data);
    int ret = m_udpServer->writeDatagram(sendDatas, QHostAddress::Broadcast, 13208);
//    qDebug() << "=================================" << ret << m_udpServer->error();
}

void UdpServer::processPendingDatagram()
{
    QHostAddress address;
    quint16 port = 0;
    // 拥有等待的数据报
    while(m_udpServer->hasPendingDatagrams())
    {
        QByteArray datagram;
        // 让datagram的大小为等待处理的数据报的大小，这样才能接收到完整的数据
        datagram.resize(m_udpServer->pendingDatagramSize());
        // 接收数据报，将其存放到datagram中
        m_udpServer->readDatagram(datagram.data(), datagram.size(), &address, &port);
    }
    if(13998 == port)
    {
        m_udpServer->writeDatagram(nullptr, address, 13999);  // UDP 发送数据
    }
    else if(13209 == port)
    {
        m_udpServer->writeDatagram(nullptr, address, 13999);  // UDP 发送数据
    }
}
