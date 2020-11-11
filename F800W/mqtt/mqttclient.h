#ifndef _MQTTCLIENT_H
#define _MQTTCLIENT_H

#include <QObject>
#include <QThread>
#include "Countdown.h"
#include "MQTTClient.h"
#include "switch.h"
#include "serverdatalist.h"

class MqttClient : public QThread
{
    Q_OBJECT
public:
    enum MqttCmd {
        UserData = 1,
        OpenDoor,
        OpenAuthority,
        DeviceUpdate,
        UploadLog,
        Unbind,
        ChangeSetting,
        Reboot = 9,
        ServerSyncEnd = 11,
        UpgradeBase64,
        SendSetupIni = 13,
        IdentifyRecord = 14,
        SetTcpServer = 15,
        ServerTime = 16,
        FactorySetup = 18,
        UpgradeTTS = 20,
        ClearOfflineData = 21,
        ClearFailFace = 22
    };
    Q_ENUM(MqttCmd)

    explicit MqttClient();
    void setPacket(ServerDataList *packet);

private:
    void init();

signals:
    void messageReceived(const QByteArray &message);

public slots:

private slots:

protected:
    virtual void run();

private:
    ServerDataList *m_packet;
};

#endif // MQTTCLIENT_H
