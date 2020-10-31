#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include <QThread>
#include <QTcpSocket>
#include <QtNetwork>
#include "switch.h"
#include "sqldatabase.h"

#define PACKET_HEAD "OFZL"

class TcpClient : public QThread
{
    Q_OBJECT
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
        DEV_ALL_PERSON_CHANGE_REQUEST = 15,
        DEV_ALL_PERSON_CHANGE_RESPONSE = 16,
        DEV_RECORD_NO_PHOTO_REQUEST = 17,
        DEV_RECORD_NO_PHOTO_RESPONE = 18,
        DEV_RECORD_PHOTO_REQUEST = 19,
        DEV_RECORD_PHOTO_RESPONE = 20,
        DEV_FACE_INSERT_FAIL_REQUEST = 21,
        DEV_FACE_INSERT_FAIL_RESPONE = 22,
        DEV_GET_ALL_IC_REQUEST = 0x19,
        DEV_GET_ALL_IC_RESPONE = 0x1A,
        DEV_ALL_PERSON_AUTH_REQUEST = 0x1B,
        DEV_ALL_PERSON_AUTH_RESPONSE = 0x1C,

        SERVER_REQUEST_CMD = 48,
        SERVER_RESPONSE_CMD = 49
    };
    explicit TcpClient();

protected:
    virtual void run();

public slots:
    // 定时向后台请求人员变动数据
    void requestUsersChange();
    // 向后台拉取全量人员id
    void requestGetAllUserID();
    // 向后台拉取全量人员权限
    void requestGetAllUserAuth();
    // 向后台拉去全量人员IC卡
    void requestGetAllUserIC();
    // 向后台请求单个人员数据
    void requestGetUsers(int id);

private:
    // 后台注册
    void requestRegister();
    // 后台登录
    void requestLogin();
    // 向服务器发送数据
    void WriteDataToServer(int msgType, QJsonObject &postObj);
    // 连接服务器
    void ConnectHost();
    // int类型大小端转换
    QByteArray intToByte(int num);
    // byte转int
    int bytesToInt(QByteArray bytes);

private slots:
    // 心跳包数据发送
    void requestHeartbeat();
    // 重连
    void Reconnect();
    // 接收后台数据
    void OnReadData();
    // 处理后台数据
    void parseData(int cmdType, QByteArray &recData);
    // 处理粘包
    void checkReadData(QByteArray readData);

private:
    QTcpSocket *m_tcpSocket;
    QTimer *m_heartbeatTimer;
    QTimer *m_requestTimer;
    QTimer *m_connectTimer;
    int m_msgLength;
    QByteArray m_msgData;
    int m_cmdType;
};
#endif // TCPCLIENT_H
