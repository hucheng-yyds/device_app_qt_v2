#include "tcpclient.h"

TcpClient::TcpClient()
{
    moveToThread(this);
}

void TcpClient::run()
{
    m_seq = 3;
    m_msgLength = 0;
    m_tcpSocket = nullptr;
    m_heartbeatTimer = new QTimer();
    m_heartbeatTimer->setInterval(30000);
    m_requestTimer = new QTimer();
    m_requestTimer->setInterval(30000);
    m_requestTimer->setSingleShot(true);
    m_connectTimer = new QTimer();
    m_connectTimer->setInterval(3000);
    m_connectTimer->setSingleShot(true);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &TcpClient::requestHeartbeat);
    connect(m_connectTimer, &QTimer::timeout, this, &TcpClient::Reconnect);
    connect(m_requestTimer, &QTimer::timeout, this, &TcpClient::Reconnect);
    exec();
}

void TcpClient::ConnectHost()
{
    m_tcpSocket = new QTcpSocket();
    connect(m_tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(ConnectError(QAbstractSocket::SocketError)));
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(OnReadData()));
    connect(m_tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(OnStateChanged(QAbstractSocket::SocketState)));

    const QString &ip = switchCtl->m_ipAddr;
    int port = switchCtl->m_tcpPort;
    qDebug() << ip << port;
    m_tcpSocket->setReadBufferSize(16*1024*1024);
    m_tcpSocket->connectToHost(ip, port);
    m_tcpSocket->waitForConnected(10000);
    if (m_tcpSocket->state() != QAbstractSocket::ConnectedState)
    {
        qDebug()<<"ip connnect fail";
        m_connectTimer->start();
    }
    else
    {
        qDebug()<<"ip connnect suc";
        system("ntpclient -s -d -c 1 -i 5 -h "+ switchCtl->m_ntpAddr.toUtf8() +" > /dev/null");
        system("hwclock -w");
        m_connectTimer->stop();
        if(!m_heartbeatTimer->isActive())
        {
            m_heartbeatTimer->start();
        }
        requestLogin();
    }
}

void TcpClient::Reconnect()
{
    qDebug() << "timer reconnect !";
    if (m_tcpSocket) {
        m_tcpSocket->abort();
        disconnect(m_tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(ConnectError(QAbstractSocket::SocketError)));
        disconnect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(OnReadData()));
        disconnect(m_tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(OnStateChanged(QAbstractSocket::SocketState)));
        m_tcpSocket->deleteLater();
        m_tcpSocket = nullptr;
        m_heartbeatTimer->stop();
    }
    ConnectHost();
}

void TcpClient::parseData(int cmdType, QByteArray &recData)
{
    QJsonParseError jsonError;
    QJsonDocument json = QJsonDocument::fromJson(recData, &jsonError);
    qDebug() << jsonError.error << cmdType;
    if (jsonError.error == QJsonParseError::NoError)
    {
        if (json.isObject())
        {
            QJsonObject jsonObj = json.object();
            if(cmdType == DEV_REGISTER_RESPONSE)
            {
                pareRegister(jsonObj);
            }
            if(cmdType == DEV_LOGIN_RESPONSE)
            {
                pareLogin(jsonObj);
            }
            if(cmdType == DEV_HEARTBEAT_RESPONSE)
            {
                pareHeartbeat(jsonObj);
            }
            if(cmdType == DEV_PERSON_RESPONSE)
            {
                parseGetUsers(jsonObj);
            }
            if(cmdType == DEV_DOOR_RECORD_RESPONSE)
            {
//                parseUploadopenlog(jsonObj);
            }
            if(cmdType == DEV_ALL_PERSON_ID_RESPONSE)
            {
                parseAllUserId(jsonObj);
            }
            if(cmdType == DEV_GET_ALL_IC_RESPONE)
            {
                parseAllIc(jsonObj);
            }
            if(cmdType == DEV_ALL_PERSON_AUTH_RESPONSE)
            {
                parseAllUserAuth(jsonObj);
            }
            if(cmdType == DEV_ALL_PERSON_CHANGE_RESPONSE)
            {
                parseUsersChange(jsonObj);
            }
            if(cmdType == SERVER_REQUEST_CMD)
            {

            }
        }
    }
}

void TcpClient::checkReadData(QByteArray readData)
{
    QByteArray recData;
    recData = readData;
    int len =0;
    int cmd = 0;
    bool ok;
    if(recData.mid(0,4) == QString("OFZL"))
    {
        /*OFLN + version(1 byte) + type(1 byte) + cmd(1byte) + msgLen(4byte) + msgBody*/
        len = recData.mid(7, 4).toHex().toInt(&ok, 16);
        cmd = bytesToInt(recData.mid(6,1));
        QByteArray msg = recData.remove(0,11);
        if(msg.size() == len)
        {
            parseData(cmd,msg);
        }
        else if(msg.size() < len)
        {
            m_msgLength = len;
            m_msgData.append(recData);
        }
        else if(msg.size() > len)
        {
            QByteArray m_msg = msg.left(len);
            parseData(cmd,m_msg);
            recData.remove(0,len);
            checkReadData(recData);
        }
    }
}

void TcpClient::OnReadData()
{
    QByteArray recData, datas;
    bool ok;
    recData.clear();
    recData = m_tcpSocket->readAll();
    m_requestTimer->stop();
    if(recData.mid(0,4) == QString("OFZL"))
    {
        /*OFLN + version(1 byte) + type(1 byte) + cmd(1byte) + msgLen(4byte) + msgBody*/
        datas.clear();
        m_msgData.clear();
        datas.append(recData);
        m_msgLength = recData.mid(7, 4).toHex().toInt(&ok, 16);
        m_cmdType = bytesToInt(recData.mid(6,1));
        QByteArray msg = recData.remove(0,11);
        if(m_msgData.size() <= m_msgLength)
        {
            m_msgData.append(msg);
        }
        if(m_msgData.size() == m_msgLength)
        {
            parseData(m_cmdType,m_msgData);
        }
        if(m_msgData.size() > m_msgLength)
        { // 解决粘包的问题
            qDebug() << "deal checkReadData";
            checkReadData(datas);
        }
    }
    else
    {
        if(recData.contains("}OFZL"))
        {
            QByteArray msg;
            msg.clear();
            int size = recData.indexOf("}OFZL");
            qDebug() << size << (m_msgLength - (m_msgData.length()));
            if((size+1) == (m_msgLength - (m_msgData.length())))
            {
                m_msgData.append(recData.left(size+1));
                qDebug() << "=================OnReadData======================" << m_msgData.size() << m_msgLength;
                if(m_msgData.size() == m_msgLength)
                {
                    m_msgLength = 0;
                    parseData(m_cmdType, m_msgData);
                    recData.remove(0,size+1);
                    if(recData.length() > 11)
                    {
                       m_msgLength = recData.mid(7,4).toHex().toInt(&ok, 16);
                       if(m_msgLength > (recData.length() - 11))
                       {
                           m_msgData.append(recData.remove(0,11));
                       }
                       else if(m_msgLength < (recData.length() -11))
                       {
                           int len = m_msgLength;
                            parseData(bytesToInt(recData.mid(6,1)), recData.remove(0,11));
                            checkReadData(recData.remove(0,len));
                       }
                       else
                       {
                           parseData(bytesToInt(recData.mid(6,1)), recData.remove(0,11));
                       }
                    }
                }
            }
            else
            {
                qDebug() << "data Error";
            }
        }
        else
        {
            m_msgData.append(recData);
            if(m_msgData.size() == m_msgLength)
            {
                parseData(m_cmdType, m_msgData);
            }
        }
    }
}

void TcpClient::pareRegister(const QJsonObject &jsonObj)
{
    int messageId = jsonObj.value("messageId").toString().toInt();
    int result = jsonObj.value("result").toInt();
    if(2 == messageId && 200 == result)
    {
        if(!m_heartbeatTimer->isActive())
        {
            m_heartbeatTimer->start();
        }
        requestLogin();
    }
}

void TcpClient::pareLogin(const QJsonObject &jsonObj)
{
    int messageId = jsonObj.value("messageId").toString().toInt();
    int result = jsonObj.value("result").toInt();
    if(2 == messageId && 200 == result)
    {
        requestGetAllUserID();
    }
}

void TcpClient::pareHeartbeat(const QJsonObject &jsonObj)
{
    if(jsonObj.contains("result"))
    {
        int result = jsonObj.value("result").toInt();
        if (401 == result)
        {
            Reconnect();
        }
    }
}

void TcpClient::parseGetUsers(const QJsonObject &jsonObj)
{
    int result = jsonObj.value("result").toInt();
    QString messageId = jsonObj.value("messageId").toString();
    if(200 == result && jsonObj.contains("data"))
    {
        QJsonValue dataValue = jsonObj.value("data");
        if(dataValue.isObject())
        {
            emit updateUsers(dataValue.toObject());
        }
    }
}

void TcpClient::parseAllUserId(const QJsonObject &jsonObj)
{
    int result = jsonObj.value("result").toInt();
    int messageId = jsonObj.value("messageId").toString().toInt();

    if (200 == result && 3 == messageId)
    {
        emit allUserId(jsonObj["data"].toArray());
    }
    else
    {
        qDebug() << "parseAllUserId result" << result << "messageId" << messageId;
    }
}

void TcpClient::parseUsersChange(const QJsonObject &jsonObj)
{
    int result = jsonObj.value("result").toInt();
    int messageId = jsonObj.value("messageId").toString().toInt();
    if(200 == result)
    {
        emit newUserId(jsonObj["updatePerson"].toArray());
    }
    else {
        qDebug() << "parseUsersChange result" << result << "messageId" << messageId;
    }
}

void TcpClient::parseAllUserAuth(const QJsonObject &jsonObj)
{
    int result = jsonObj.value("result").toInt();
    int messageId = jsonObj.value("messageId").toString().toInt();
    if(200 == result)
    {
        emit allUserAuth(jsonObj["data"].toArray());
    }
    else {
        qDebug() << "parseAllUserAuth result" << result << "messageId" << messageId;
    }
}


void TcpClient::parseAllIc(const QJsonObject &jsonObj)
{
    int result = jsonObj.value("result").toInt();
    int messageId = jsonObj.value("messageId").toString().toInt();
    if(200 == result)
    {
        emit allUserIc(jsonObj["data"].toArray());
    }
    else {
        qDebug() << "parseAllUserAuth result" << result << "messageId" << messageId;
    }
}

void TcpClient::requestRegister()
{
    QString rec_passwd = switchCtl->m_passwd;
    QString sn = switchCtl->m_sn;
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time = QDateTime::currentDateTime().addSecs(28800);
    QString timestamp = QString("%1").arg(origin_time.secsTo(current_date_time));
    QString sign = sn + rec_passwd + timestamp + PACKET_HEAD;
    QByteArray hash = QCryptographicHash::hash(sign.toLatin1(), QCryptographicHash::Md5);
    QJsonObject dataObj, obj;
    sign = hash.toHex();
    dataObj.insert("sn", sn);
    dataObj.insert("messageId", "2");
    dataObj.insert("message", "registerReq");
    obj.insert("timestamp", timestamp.toInt());
    dataObj.insert("type", DEVICE_TYPE);
    obj.insert("sign", sign);
    dataObj.insert("data", obj);
    WriteDataToServer(DEV_REGISTER_REQUEST, dataObj);
}

void TcpClient::requestLogin()
{
    QString rec_passwd = switchCtl->m_passwd;
    QString sn = switchCtl->m_sn;
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time = QDateTime::currentDateTime().addSecs(28800);
    QString timestamp = QString("%1").arg(origin_time.secsTo(current_date_time));
    QString sign = sn + rec_passwd + timestamp + PACKET_HEAD;
    QByteArray hash = QCryptographicHash::hash(sign.toLatin1(), QCryptographicHash::Md5);
    QJsonObject dataObj, obj;
    sign = hash.toHex();
    dataObj.insert("sn", sn);
    dataObj.insert("messageId", 1);
    dataObj.insert("message", "loginReq");
    obj.insert("timestamp", timestamp.toInt());
    obj.insert("dversion", "2");
    obj.insert("sign", sign);
    dataObj.insert("data", obj);

    WriteDataToServer(DEV_LOGIN_REQUEST, dataObj);
}

void TcpClient::requestHeartbeat()
{
    int count = sqlDatabase->m_localFaceSet.size();
    QJsonObject dataObj, obj;
    dataObj.insert("messageId", QString("%1").arg(m_seq++));
    dataObj.insert("message", "heartbeatReq");
    obj.insert("sn", switchCtl->m_sn);
    obj.insert("peopleCount", count);
    obj.insert("capacity", 30000);
    obj.insert("appVersion", VERSION);
    obj.insert("networkName", "eth0");
    obj.insert("ipAddr", switchCtl->m_ipAddr);
    obj.insert("languageSet", switchCtl->m_language);
    dataObj.insert("data", obj);

    WriteDataToServer(DEV_HEARTBEAT_REQUEST, dataObj);
}

void TcpClient::requestUsersChange()
{
    QJsonObject dataObj;
    dataObj.insert("messageId", QString("%1").arg(m_seq++));
    dataObj.insert("message", "regularUserReq");
    dataObj.insert("sn", switchCtl->m_sn);
    qDebug() << "requestUsersChange" << dataObj;
    WriteDataToServer(DEV_ALL_PERSON_CHANGE_REQUEST, dataObj);
}

void TcpClient::requestGetAllUserID()
{
    QJsonObject dataObj;
    dataObj.insert("messageId", 3);
    dataObj.insert("message", "allUserReq");
    dataObj.insert("sn", switchCtl->m_sn);
    qDebug() << "requestGetAllUserID" << dataObj;
    WriteDataToServer(DEV_ALL_PERSON_ID_REQUEST,dataObj);
}

void TcpClient::requestGetUsers(int id)
{
    QJsonObject dataObj, obj;
    QString oldPhotoName = "";
    dataObj.insert("messageId", QString("%1").arg(id));
    dataObj.insert("message", "getUserReq");
    dataObj.insert("sn", switchCtl->m_sn);
    obj.insert("mid", id);
    obj.insert("photoName", oldPhotoName);
    dataObj.insert("data", obj);

    WriteDataToServer(DEV_PERSON_REQUEST,dataObj);
}

void TcpClient::requestGetAllUserAuth()
{
    QJsonObject dataObj;
    dataObj.insert("messageId", QString("%1").arg(m_seq++));
    dataObj.insert("message", "allUserAuthReq");
    dataObj.insert("sn", switchCtl->m_sn);
    qDebug() << "requestGetAllUserAuth" << dataObj;
    WriteDataToServer(DEV_ALL_PERSON_AUTH_REQUEST,dataObj);
}

void TcpClient::requestGetAllUserIC()
{
    QJsonObject dataObj;
    dataObj.insert("messageId", QString("%1").arg(m_seq++));
    dataObj.insert("message", "icReq");
    dataObj.insert("sn", switchCtl->m_sn);
    qDebug() << "requestGetAllUserIC" << dataObj;
    WriteDataToServer(DEV_GET_ALL_IC_REQUEST,dataObj);
}

int TcpClient::bytesToInt(QByteArray bytes)
{
    int addr = bytes[0] & 0x000000FF;
    addr |= ((bytes[1] << 8) & 0x0000FF00);
    addr |= ((bytes[2] << 16) & 0x00FF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
}

QByteArray TcpClient::intToByte(int num)
{
    QByteArray abyte0;
    abyte0.resize(4);
    abyte0[0] = (uchar) ((0xff000000 & num) >> 24);
    abyte0[1] = (uchar) ((0x00ff0000 & num) >> 16);
    abyte0[2] = (uchar) ((0x0000ff00 & num) >> 8);
    abyte0[3] = (uchar) (0x000000ff & num);
    return abyte0;
}

void TcpClient::WriteDataToServer(int msgType, QJsonObject &postObj)
{
    if (msgType != DEV_LOGIN_REQUEST && !m_tcpSocket)
    {
        if (!m_requestTimer->isActive())
        {
            m_requestTimer->start();
        }
        return ;
    }
    QByteArray data;
    QByteArray sendData;
    QJsonDocument document;
    document.setObject(postObj);
    data = document.toJson(QJsonDocument::Compact);
    sendData.clear();
    sendData.append(PACKET_HEAD);
    sendData.append(uchar(2));
    sendData.append(uchar(1));
    sendData.append(uchar(msgType));
    sendData.append(intToByte(data.length()));
    sendData.append(data);

    int backT = m_tcpSocket->write(sendData);  //发送数据到服务端
    qDebug() << "cmdType:" << msgType << "size:" << backT;
    //以下是为了防止发送数据的同时读取数据出现问题
    m_tcpSocket->flush();
    if (!m_requestTimer->isActive())
    {
        m_requestTimer->start();
    }
}
