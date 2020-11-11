#include "mqttclient.h"
#include <QNetworkConfigurationManager>
#include <QtNetwork>
#define PORT 61883
#define HOST "master.api.officelinking.com"

class IPStack
{
public:
    IPStack()
    {

    }

    int connect(const char* hostname, int port)
    {
        int type = SOCK_STREAM;
        struct sockaddr_in address;
        int rc = -1;
        sa_family_t family = AF_INET;
        struct addrinfo *result = NULL;
        struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};

        if ((rc = getaddrinfo(hostname, NULL, &hints, &result)) == 0)
        {
            struct addrinfo* res = result;

            /* prefer ip4 addresses */
            while (res)
            {
                if (res->ai_family == AF_INET)
                {
                    result = res;
                    break;
                }
                res = res->ai_next;
            }

            if (result->ai_family == AF_INET)
            {
                address.sin_port = htons(port);
                address.sin_family = family = AF_INET;
                address.sin_addr = ((struct sockaddr_in*)(result->ai_addr))->sin_addr;
            }
            else
                rc = -1;

            freeaddrinfo(result);
        }

        if (rc == 0)
        {
            mysock = socket(family, type, 0);
            if (mysock != -1)
            {
                rc = ::connect(mysock, (struct sockaddr*)&address, sizeof(address));
            }
        }

        return rc;
    }

    int read(unsigned char* buffer, int len, int timeout_ms)
    {
        struct timeval interval = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};
        if (interval.tv_sec < 0 || (interval.tv_sec == 0 && interval.tv_usec <= 0))
        {
            interval.tv_sec = 0;
            interval.tv_usec = 100;
        }

        setsockopt(mysock, SOL_SOCKET, SO_RCVTIMEO, (char *)&interval, sizeof(struct timeval));

        int bytes = 0;
        int i = 0; const int max_tries = 10;
        while (bytes < len)
        {
            int rc = ::recv(mysock, &buffer[bytes], (size_t)(len - bytes), 0);
            if (rc == -1)
            {
                if (errno != EAGAIN && errno != EWOULDBLOCK)
                    bytes = -1;
                break;
            }
            else
                bytes += rc;
            if (++i >= max_tries)
                break;
            if (rc == 0)
                break;
        }
        return bytes;
    }

    int write(unsigned char* buffer, int len, int timeout)
    {
        struct timeval tv;

        tv.tv_sec = 0;  /* 30 Secs Timeout */
        tv.tv_usec = timeout * 1000;  // Not init'ing this can cause strange errors

        setsockopt(mysock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));
        int	rc = ::write(mysock, buffer, len);
        //printf("write rc %d\n", rc);
        return rc;
    }

    int disconnect()
    {
        return ::close(mysock);
    }

private:

    int mysock;
};

QQueue<QByteArray> g_payload;
QByteArray g_topic;

MqttClient::MqttClient()
{
    moveToThread(this);
}

void MqttClient::setPacket(ServerDataList *packet)
{
    m_packet = packet;
}

void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;

    qDebug("Payload %.*s\n", (int)message.payloadlen, (char*)message.payload);
    g_payload.enqueue(QByteArray((char*)message.payload, (int)message.payloadlen));
}

void myconnect(IPStack& ipstack, MQTT::Client<IPStack, Countdown, 1000>& client, MQTTPacket_connectData& data, const char* host)
{
    int rc = ipstack.connect(host, PORT);
    if (rc != 0)
    {
        qDebug() << "rc from TCP connect is" << rc;
    }
    rc = client.connect(data);
    if (rc != 0)
    {
        qDebug() << "Failed to connect, return code" << rc;
        return ;
    }

    rc = client.subscribe(g_topic, MQTT::QOS0, messageArrived);
    qDebug() << "Subscribed" << rc << g_topic;
}

void MqttClient::init()
{
    const char* host = HOST;
    g_topic = "device/" + switchCtl->m_sn.toUtf8();

    IPStack ipstack = IPStack();
    MQTT::Client<IPStack, Countdown, 1000> client = MQTT::Client<IPStack, Countdown, 1000>(ipstack);

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.clientID.cstring = g_topic.data();
    data.username.cstring = (char *)"admin";
    data.password.cstring = (char *)"ofzl";

    qDebug() << data.username.cstring << data.password.cstring;
    data.keepAliveInterval = 10;
    printf("will flag %d\n", data.willFlag);

    myconnect(ipstack, client, data, host);

    while (true)
    {
        client.yield(1000);

        if (!g_payload.isEmpty())
        {
            FacePacketNode_t *packet = new FacePacketNode_t;
            packet->datas = g_payload.dequeue();
            m_packet->PushLogPacket(packet);
        }
        if (!client.isConnected())
        {
            qDebug() << "client.isConnected:" << client.isConnected();
            qDebug() << client.disconnect();
            qDebug() << ipstack.disconnect();
            myconnect(ipstack, client, data, host);
        }
    }
}

void MqttClient::run()
{
    init();
    exec();
}
