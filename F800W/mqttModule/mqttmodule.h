#ifndef MQTTMODULE_H
#define MQTTMODULE_H
#include <QThread>
#include "MQTTClient.h"
#include "switch.h"
#include "datashare.h"
#include "serverdatalist.h"

class MqttModule : public QThread
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
        IC = 17,
        FactorySetup = 18,
        UpgradeTTS = 20,
        ClearOfflineData = 21,
        ClearFailFace = 22,
        Bind = 23
    };
    Q_ENUM(MqttCmd)
    explicit MqttModule();
    void setPacket(ServerDataList *dataList);

protected:
    virtual void run();

private:
    MQTTClient m_mqttClient;
    QString m_serverUrl;
    QString m_topic;
    ServerDataList *m_packet;
};
#endif // MQTTMODULE_H
