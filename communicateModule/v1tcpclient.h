#ifndef V1TCPCLIENT_H
#define V1TCPCLIENT_H
#include <QThread>
#include <QTcpSocket>
#include <QtNetwork>
#include "switch.h"
#include "sqldatabase.h"
#include "serverdatadeal.h"

class V1TcpClient : public QThread
{
    Q_OBJECT
protected:
    void run();

public:
    enum ClientType {
        DEV_LOGIN_REQUEST = 1,
        DEV_LOGIN_RESPONSE = 2,
        DEV_REGISTER_REQUEST = 5,
        DEV_REGISTER_RESPONSE = 6,
        DEV_HEARTBEAT_REQUEST = 7,
        DEV_HEARTBEAT_RESPONSE = 8,
        DEV_PERSON_REQUEST = 9,
        DEV_PERSON_RESPONSE = 10,
        DEV_ALL_PERSON_ID_REQUEST = 11,
        DEV_ALL_PERSON_ID_RESPONSE = 12,
        DEV_DOOR_RECORD_REQUEST = 13,
        DEV_DOOR_RECORD_RESPONSE = 14,
        DEV_RECORD_NO_PHOTO_REQUEST = 17,
        DEV_RECORD_NO_PHOTO_RESPONE = 18,
        DEV_RECORD_PHOTO_REQUEST = 19,
        DEV_RECORD_PHOTO_RESPONE = 20,
        DEV_FACE_INSERT_FAIL_REQUEST = 21,
        DEV_FACE_INSERT_FAIL_RESPONE = 22,
        DEV_GET_ALL_NUMBER_REQUEST = 0x19,
        DEV_GET_ALL_NUMBER_RESPONE = 0x1A,
        DEV_RECORD_ALARM_REQUEST = 0x1B,
        DEV_RECORD_ALARM_RESPONE = 0x1C,

        SERVER_REQUEST_CMD = 48,
        SERVER_RESPONSE_CMD = 49
    };

    explicit V1TcpClient();
    // 处理后台服务器数据入口
    void setPacket(ServerDataList *packet);


public slots:
    void requestRegister();
    void requestDoortoken();
    void requestHeartbeat();
    void requestGetUsers(int id);
    void requestGetAllUserID();
    void responseServer(const QString &type, const QString &messageId, const QJsonObject &jsonData);
    void requestInserFail();
    void requestGetAllIC();
    void requestUploadAlarm(const QString &type);
    void uploadopenlog(int id, int userId, const QString &photo, int isOver,int type, int isTemp, int sex, const QStringList &datas);
    void uploadOffine(int id, int userId, const QString &photo, int isOver,int type, int isTemp, int sex, const QStringList &datas);

private slots:
    int getTimeZoneMin();
    int getTimeZone();
    void OnReadData();
    void OnStateChanged(QAbstractSocket::SocketState state);
    void ConnectHost();
    void ConnectError(QAbstractSocket::SocketError state);
    void Reconnect();
    void WriteDataToServer(int msgType, QJsonObject &postObj);

    QByteArray intToByte(int num);
    int bytesToInt(QByteArray bytes);
    void parseData(int cmdType, QByteArray &recData);
    bool checkPacketHead(const QByteArray &packet);

    void parseRegister(const QJsonObject &rootObj);
    void parseDoortoken(const QJsonObject &rootObj);
    void parseHeartbeat(const QJsonObject &rootObj);
    void parseGetUsers(const QJsonObject &rootObj);
    void parseGetAllUserID(const QJsonObject &rootObj);
    void parseUploadopenlog(const QJsonObject &rootObj);
    void parseRecord(bool isPhoto, const QJsonObject &rootObj);
    void parseInsertFail(const QJsonObject &rootObj);
    void parseGetCmd(QByteArray msgBody);
    void parseGetAllIC(const QJsonObject &rootObj);
    void parseUploadAlarm(const QJsonObject &rootObj);

    void responseServerSetup();
    void checkReadData(QByteArray readData);

signals:
    void allUserId(const QJsonArray &jsonArr);
    void allIC(const QJsonArray &jsonArr);
    void insertIC(const QJsonArray &jsonArr);
    void updateUsers(const QJsonObject &jsonObj);
    void messageReceived(QByteArray msgBody);

private:
    QTcpSocket *m_tcpSocket;
    QTimer *m_heartbeatTimer;
    QTimer *m_requestTimer;
    QTimer *m_connectTimer;
    int m_msgLength;
    QByteArray m_msgData;
    int m_cmdType;
    int m_seq;
    ServerDataList *m_serverData;
};
#endif // V1TCPCLIENT_H
