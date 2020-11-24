#include <QQueue>
#include "mqttmodule.h"

QQueue<QByteArray> g_payload;
bool g_recvDataOk = false;

MqttModule::MqttModule()
{
    m_serverUrl = "tcp://master.api.officelinking.com:61883";
    m_topic = "device/" + switchCtl->m_sn;
}

void MqttModule::setPacket(ServerDataList *dataList)
{
    m_packet = dataList;
}

static void deliveryComplete(void* context, MQTTClient_deliveryToken dt)
{

}

static int messageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* m)
{
    if(!g_recvDataOk)
    {
        g_payload.enqueue(QByteArray((char*)m->payload, (int)m->payloadlen));
        g_recvDataOk = true;
    }
    else {
        qDebug() << "messageArrived";
    }
    MQTTClient_free(topicName);
    MQTTClient_freeMessage(&m);
    return 1;
}

void MqttModule::run()
{
    QString name = "admin";
    QString psd = "ofzl";
    bool status = false;
    char **urls = (char **)malloc(sizeof(char*) * 5);
    urls[0] = (char *)malloc(m_serverUrl.size() + 1);
    strcpy(urls[0], m_serverUrl.toUtf8().data());
    MQTTClient_connectOptions opts = MQTTClient_connectOptions_initializer;
    MQTTClient_willOptions wopts =  MQTTClient_willOptions_initializer;
    opts.keepAliveInterval = 20;
    opts.cleansession = 1;
    opts.username = "admin";//name.toUtf8();
    opts.password = "ofzl";//psd.toUtf8();
    opts.serverURIs = urls;
    opts.MQTTVersion = MQTTVERSION_DEFAULT;
    opts.will = &wopts;
    opts.will->message = "willmessage";
    opts.will->qos = 1;
    opts.will->retained = 0;
    opts.will->topicName = "willtopic";
    opts.will = nullptr;
    while(true)
    {
        if(!status)
        {
            int rc = MQTTClient_create(&m_mqttClient, m_serverUrl.toUtf8(), switchCtl->m_sn.toUtf8(), MQTTCLIENT_PERSISTENCE_DEFAULT, nullptr);
            if (rc != MQTTCLIENT_SUCCESS)
            {
                MQTTClient_destroy(&m_mqttClient);
                sleep(2);
                continue;
            }
            rc = MQTTClient_setCallbacks(m_mqttClient, nullptr, nullptr, messageArrived, deliveryComplete);
            if(rc != MQTTCLIENT_SUCCESS)
            {
                qt_debug() << "MQTTClient_setCallbacks fail";
                sleep(2);
                continue;
            }
            rc = MQTTClient_connect(m_mqttClient, &opts);
            if(rc != MQTTCLIENT_SUCCESS)
            {
                qt_debug() << "MQTTClient_connect fail";
                sleep(2);
                continue;
            }
            rc = MQTTClient_subscribe(m_mqttClient, m_topic.toUtf8(), 2);
            if(rc != MQTTCLIENT_SUCCESS)
            {
                qt_debug() << "MQTTClient_subscribe fail";
                sleep(2);
                continue;
            }
            qt_debug() << "mqtt connect sccess";
            status = true;
            sleep(1);
        }
        int ret = MQTTClient_isConnected(m_mqttClient);
        if(ret > 0)
        {
        }
        else {
            qt_debug() << "mqtt reconnect";
            int rc = MQTTClient_unsubscribe(m_mqttClient, m_topic.toUtf8());
            rc = MQTTClient_disconnect(m_mqttClient, 0);
            if(rc != MQTTCLIENT_SUCCESS)
            {
                qt_debug() << "MQTTClient_disconnect fail" << rc;
                sleep(2);
                continue;
            }
            sleep(1);
            rc = MQTTClient_connect(m_mqttClient, &opts);
            if(rc != MQTTCLIENT_SUCCESS)
            {
                qt_debug() << "MQTTClient_connect fail" << rc;
                sleep(2);
                continue;
            }
            rc = MQTTClient_subscribe(m_mqttClient, m_topic.toUtf8(), 2);
            if(rc != MQTTCLIENT_SUCCESS)
            {
                qt_debug() << "MQTTClient_subscribe fail" << rc;
                sleep(2);
                continue;
            }
            sleep(5);
        }
        if(g_recvDataOk)
        {
            FacePacketNode_t *packetNode = new FacePacketNode_t;
            packetNode->datas = g_payload.dequeue();
            m_packet->PushLogPacket(packetNode);
            g_recvDataOk = false;
        }
        msleep(500);
    }
}
