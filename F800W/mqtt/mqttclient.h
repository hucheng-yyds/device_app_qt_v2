#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H
#include <QThread>
#include "qmqtt.h"

class MqttClient : public QThread
{
    Q_OBJECT
public:
    explicit MqttClient();

protected:
    void run();

private:
    void connectMqtt();
    void disConnect();
    void reconnect();

private slots:
    void onMQTT_Connected();
    void onMQTT_DisConnected();
    void error(const QMQTT::ClientError error);
    void onMQTT_Received(const QMQTT::Message &message);
    void onMQTT_subscribed(const QString& topic);

private:
    QMQTT::Client * m_client;
};
#endif // MQTTCLIENT_H
