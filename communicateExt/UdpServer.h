#ifndef UDPSERVER_H
#define UDPSERVER_H
#include <QtNetwork>
#include <QThread>
#include "switch.h"

class QUdpSocket;


class UdpServer:public QThread
{
    Q_OBJECT

public:
    explicit UdpServer();

public slots:

protected:
    void run();

private:
    void sendData();

private slots:
    void processPendingDatagram();// 读取接收到的数据报

private:
    QUdpSocket *m_udpServer;

};
#endif // UDPSERVER_H
