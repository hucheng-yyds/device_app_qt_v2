#include "mqttclient.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTimer>

extern QString ipAddr;
using namespace QMQTT;
extern bool networking;

MqttClient::MqttClient()
{
    moveToThread(this);
}

void MqttClient::connectMqtt()
{
    m_client = new Client();
    connect(m_client, SIGNAL(connected()), this, SLOT(onMQTT_Connected()));
    connect(m_client, SIGNAL(disconnected()), this, SLOT(onMQTT_DisConnected()));
    connect(m_client,SIGNAL(received(const QMQTT::Message&)),this,SLOT(onMQTT_Received(const QMQTT::Message&)));
    connect(m_client,SIGNAL(error(const QMQTT::ClientError)),this,SLOT(error(const QMQTT::ClientError)));
    connect(m_client,SIGNAL(subscribed(const QString&)),this,SLOT(onMQTT_subscribed(const QString&)));
    connect(m_client, SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(OnStateChanged(QAbstractSocket::SocketState)));
    m_client->setPort(61883);
    m_client->setHostName("master.api.officelinking.com");
    m_client->setWillQos(1);
    m_client->setUsername("admin");
    m_client->setPassword("ofzl");
//    m_client->setClientId(m_deviceId);
    m_client->setKeepAlive(10);
//    m_client->setWillRetain(false);
//    m_client->setWillMessage(document.toJson());
    m_client->connectToHost();
    qDebug() << "===============================================================11111111111111111111111111" << m_client->connectionState();
}

void MqttClient::run()
{

//    QTimer *timer = new QTimer(this);
//    connect(timer, &QTimer::timeout, this, &MqttClient::sendHeart);
//    timer->setInterval(60000);
//    timer->start();
    exec();
}

void MqttClient::reconnect()
{
    if(m_client)
    {
        m_client->disconnectFromHost();
        disconnect(m_client, SIGNAL(connected()), this, SLOT(onMQTT_Connected()));
        disconnect(m_client, SIGNAL(disconnected()), this, SLOT(onMQTT_DisConnected()));
        disconnect(m_client,SIGNAL(received(const QMQTT::Message&)),this,SLOT(onMQTT_Received(const QMQTT::Message&)));
        disconnect(m_client,SIGNAL(subscribed(const QString&)),this,SLOT(onMQTT_subscribed(const QString&)));
        delete m_client;
        m_client = nullptr;
    }
    connectMqtt();
}

void MqttClient::disConnect()
{
    m_client->disconnectFromHost();
    disconnect(m_client, SIGNAL(connected()), this, SLOT(onMQTT_Connected()));
    disconnect(m_client, SIGNAL(disconnected()), this, SLOT(onMQTT_DisConnected()));
    disconnect(m_client,SIGNAL(received(const QMQTT::Message&)),this,SLOT(onMQTT_Received(const QMQTT::Message&)));
    disconnect(m_client,SIGNAL(subscribed(const QString&)),this,SLOT(onMQTT_subscribed(const QString&)));
    delete m_client;
    m_client = nullptr;
}

void MqttClient::error(const QMQTT::ClientError error)
{

}

void MqttClient::onMQTT_DisConnected()
{

}

void MqttClient::onMQTT_Connected()
{

}

void MqttClient::onMQTT_Received(const QMQTT::Message &message)
{

}

void MqttClient::onMQTT_subscribed(const QString &topic)
{
    qDebug() << "onMQTT_subscribed" << topic;
}
