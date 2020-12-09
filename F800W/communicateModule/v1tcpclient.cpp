#include "v1tcpclient.h"

#define PACKET_HEAD "OFZL"

V1TcpClient::V1TcpClient()
{
    m_msgLength = 0;
    moveToThread(this);
}

void V1TcpClient::run()
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
    m_connectTimer->setInterval(5000);
    m_connectTimer->setSingleShot(true);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &V1TcpClient::requestHeartbeat);
    connect(m_connectTimer, &QTimer::timeout, this, &V1TcpClient::Reconnect);
    connect(m_requestTimer, &QTimer::timeout, this, &V1TcpClient::Reconnect);
    ConnectHost();
    exec();
}

void V1TcpClient::setPacket(ServerDataList *packet)
{
    m_serverData = packet;
}

void V1TcpClient::responseServer(const QString &type, const QString &messageId, const QJsonObject &jsonData)
{
    QByteArray data;
    QByteArray sendData;
    QJsonDocument document;
    document.setObject(jsonData);
    data = document.toJson(QJsonDocument::Compact);
//    qt_debug() << data;
    sendData.clear();
    sendData.append(PACKET_HEAD);
    sendData.append(uchar(1));
    sendData.append(uchar(1));
    sendData.append(uchar(SERVER_RESPONSE_CMD));
    sendData.append(intToByte(data.length()));
    sendData.append(data);

    int backT = m_tcpSocket->write(sendData);  //发送数据到服务端
    qt_debug() << backT;
    //以下是为了防止发送数据的同时读取数据出现问题
    m_tcpSocket->flush();
}

QByteArray V1TcpClient::intToByte(int num)
{
//    qt_debug()<<num;
    QByteArray abyte0;
    abyte0.resize(4);
    abyte0[0] = (uchar) ((0xff000000 & num) >> 24);
    abyte0[1] = (uchar) ((0x00ff0000 & num) >> 16);
    abyte0[2] = (uchar) ((0x0000ff00 & num) >> 8);
    abyte0[3] = (uchar) (0x000000ff & num);
//    qt_debug()<<abyte0<<(0x000000ff & num)<<((0x0000ff00 & num) >> 8)<<((0x00ff0000 & num) >> 16)<<((0xff000000 & num) >> 24);
    return abyte0;
}

int V1TcpClient::bytesToInt(QByteArray bytes)
{
    int addr = bytes[0] & 0x000000FF;
    addr |= ((bytes[1] << 8) & 0x0000FF00);
    addr |= ((bytes[2] << 16) & 0x00FF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
}

void V1TcpClient::ConnectHost()
{
    m_tcpSocket = new QTcpSocket();
    connect(m_tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(ConnectError(QAbstractSocket::SocketError)));
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(OnReadData()));
    connect(m_tcpSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(OnStateChanged(QAbstractSocket::SocketState)));

    const QString &ip = switchCtl->m_tcpAddr;
    int port = switchCtl->m_tcpPort;
    qt_debug() << ip << port;
    m_tcpSocket->setReadBufferSize(16*1024*1024);
    m_tcpSocket->connectToHost(ip, port);
    m_tcpSocket->waitForConnected(10000);
    if (m_tcpSocket->state() != QAbstractSocket::ConnectedState)
    {
        qt_debug()<<"ip connnect fail";
        m_requestTimer->start();
        dataShare->m_netStatus = false;
    }
    else
    {
        qt_debug()<<"ip connnect suc";
        system("ntpclient -s -d -c 1 -i 5 -h "+ switchCtl->m_ntpAddr.toUtf8() +" > /dev/null");
        system("hwclock -w");
        m_requestTimer->stop();
        if(!m_heartbeatTimer->isActive())
        {
            m_heartbeatTimer->start();
        }
        requestDoortoken();
    }
}

void V1TcpClient::Reconnect()
{
    qt_debug() << "timer reconnect !";
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

void V1TcpClient::ConnectError(QAbstractSocket::SocketError state)
{
    if(QAbstractSocket::RemoteHostClosedError == state)
    {
    }
}

void V1TcpClient::OnStateChanged(QAbstractSocket::SocketState state)
{
    qt_debug()<<state;
    if(state == QAbstractSocket::UnconnectedState)
    {
    }
}

void V1TcpClient::parseData(int cmdType, QByteArray &recData)
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
                parseRegister(jsonObj);
            }
            if(cmdType == DEV_LOGIN_RESPONSE)
            {
                parseDoortoken(jsonObj);
            }
            if(cmdType == DEV_HEARTBEAT_RESPONSE)
            {
                parseHeartbeat(jsonObj);
            }
            if(cmdType == DEV_PERSON_RESPONSE)
            {
                parseGetUsers(jsonObj);
            }
            if(cmdType == DEV_DOOR_RECORD_RESPONSE)
            {
                parseUploadopenlog(jsonObj);
            }
            if(cmdType == DEV_ALL_PERSON_ID_RESPONSE)
            {
                parseGetAllUserID(jsonObj);
            }
            if(cmdType == SERVER_REQUEST_CMD)
            {
                FacePacketNode_t *pack = new FacePacketNode_t;
                pack->datas = recData;
                m_serverData->PushLogPacket(pack);
                parseGetCmd(recData);
            }
            if(cmdType == DEV_RECORD_PHOTO_RESPONE)
            {
                parseRecord(true,jsonObj);
            }
            if(cmdType == DEV_RECORD_NO_PHOTO_RESPONE)
            {
                parseRecord(false, jsonObj);
            }
            if(cmdType == DEV_FACE_INSERT_FAIL_RESPONE)
            {
                parseInsertFail(jsonObj);
            }
            if (cmdType == DEV_GET_ALL_NUMBER_RESPONE) {
                parseGetAllIC(jsonObj);
            }
            if (cmdType == DEV_RECORD_ALARM_RESPONE) {
                parseUploadAlarm(jsonObj);
            }
        }
        else {
            qDebug() << "==++++++++++++++++";
        }
    }
}

bool V1TcpClient::checkPacketHead(const QByteArray &packet)
{
    return packet.left(4) == PACKET_HEAD;
}

void V1TcpClient::checkReadData(QByteArray readData)
{
    QByteArray recData;
    recData = readData;
    int len =0;
    int cmd = 0;
    bool ok;
//    qt_debug() << readData;
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

void V1TcpClient::OnReadData()
{
    QByteArray recData, datas;
    bool ok;
    recData.clear();
    recData = m_tcpSocket->readAll();
    m_connectTimer->stop();
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
            qt_debug() << "deal checkReadData";
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
            qt_debug() << size << (m_msgLength - (m_msgData.length()));
            if((size+1) == (m_msgLength - (m_msgData.length())))
            {
                m_msgData.append(recData.left(size+1));
                qt_debug() << "=================OnReadData======================" << m_msgData.size() << m_msgLength;
                if(m_msgData.size() == m_msgLength)
                {
                    m_msgLength = 0;
                    parseData(m_cmdType, m_msgData);
                    recData.remove(0,size+1);
//                    qt_debug() << recData;
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
                            parseData(bytesToInt(recData.mid(6,1)),recData.remove(0,11));
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
                qt_debug() << "data Error";
            }
        }
        else {
            m_msgData.append(recData);
            if(m_msgData.size() == m_msgLength)
            {
                parseData(m_cmdType,m_msgData);
            }
        }
    }
}

void V1TcpClient::WriteDataToServer(int msgType, QJsonObject &postObj)
{
    if (msgType != DEV_LOGIN_REQUEST && !m_tcpSocket)
    {
        if (!m_requestTimer->isActive())
        {
            if (m_connectTimer->isActive())
            {
                m_connectTimer->stop();
            }
            m_requestTimer->start();
        }
        return ;
    }
    QByteArray data;
    QByteArray sendData;
    QJsonDocument document;
    document.setObject(postObj);
    data = document.toJson(QJsonDocument::Compact);
//    qt_debug() << data;
    sendData.clear();
    sendData.append(PACKET_HEAD);
    sendData.append(uchar(1));
    sendData.append(uchar(1));
    sendData.append(uchar(msgType));
    sendData.append(intToByte(data.length()));
    sendData.append(data);

    int backT = m_tcpSocket->write(sendData);  //发送数据到服务端
    qt_debug() << "cmdType:" << msgType << "size:" << backT;
    //以下是为了防止发送数据的同时读取数据出现问题
    m_tcpSocket->flush();
    if (!m_connectTimer->isActive())
    {
        m_connectTimer->start();
    }
}


void V1TcpClient::requestInserFail()
{
    if(sqlDatabase->m_localFaceFail.size() > 0)
    {
        QJsonObject jsonObj;
        QJsonArray jsonAry;
        QJsonObject dataObj;
        jsonObj.insert("sn", switchCtl->m_sn);
        foreach(int num, sqlDatabase->m_localFaceFail)
        {
            dataObj.insert("mid",num);
            dataObj.insert("type", sqlDatabase->sqlSelectFaile(num));
            jsonAry.append(dataObj);
        };
        sqlDatabase->sqlDeleteAllFail();
        jsonObj.insert("data",jsonAry);
        WriteDataToServer(DEV_FACE_INSERT_FAIL_REQUEST,jsonObj);
    }
}

void V1TcpClient::requestGetAllIC()
{
    qt_debug();
    QJsonObject jsonObj;
    jsonObj.insert("sn", switchCtl->m_sn);
    WriteDataToServer(DEV_GET_ALL_NUMBER_REQUEST, jsonObj);
}

void V1TcpClient::requestUploadAlarm(const QString &type)
{
    QJsonObject jsonObj;
    QJsonObject dataObj;
    dataObj.insert("alarmType", type);
    jsonObj.insert("data", dataObj);
    jsonObj.insert("sn", switchCtl->m_sn);
    jsonObj.insert("message", "alarmReq");
    jsonObj.insert("messageId", "11");
    WriteDataToServer(DEV_RECORD_ALARM_REQUEST, jsonObj);
}

void V1TcpClient::requestRegister()
{
    qDebug()<<__PRETTY_FUNCTION__<<switchCtl->m_sn;
    QDateTime current_date_time =QDateTime::currentDateTime().addSecs(28800);
    QString timestamp =current_date_time.toString("yyyyMMddhhmmss");
    QJsonObject dataObj;
    dataObj.insert("sn",switchCtl->m_sn);
    dataObj.insert("timestamp", timestamp.toInt());
    dataObj.insert("type",DEVICE_TYPE);

    WriteDataToServer(DEV_REGISTER_REQUEST,dataObj);
}

void V1TcpClient::requestDoortoken()
{
    QString rec_passwd = switchCtl->m_passwd;
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time =QDateTime::currentDateTime().addSecs(28800);
    QString timestamp =QString("%1").arg(origin_time.secsTo(current_date_time));
    QString sign = switchCtl->m_sn + rec_passwd + timestamp/* + PACKET_HEAD*/;
    QByteArray hash = QCryptographicHash::hash(sign.toLatin1(), QCryptographicHash::Md5);
    QJsonObject dataObj;
    qDebug() << sign << hash.toHex();
    sign = hash.toHex();
    dataObj.insert("sn", switchCtl->m_sn);
    dataObj.insert("timestamp", timestamp.toInt());
    dataObj.insert("dversion", "1");
    dataObj.insert("sign", sign);
    WriteDataToServer(DEV_LOGIN_REQUEST,dataObj);
}

void V1TcpClient::requestHeartbeat()
{
    int count = sqlDatabase->m_localFaceSet.size();
    QJsonObject dataObj;
    dataObj.insert("sn",switchCtl->m_sn);
    dataObj.insert("peopleCount", count);
    dataObj.insert("capacity",50000);
    dataObj.insert("wg", 0);
    dataObj.insert("appVersion", VERSION);
    dataObj.insert("networkName", "eth0");
    dataObj.insert("ipAddr", dataShare->m_ipAddr);
    dataObj.insert("languageSet", switchCtl->m_language);

    WriteDataToServer(DEV_HEARTBEAT_REQUEST,dataObj);
}

void V1TcpClient::requestGetUsers(int id)
{
    QJsonObject dataObj;
//    QUuid uuid = QUuid::createUuid();
//    QString strId = uuid.toString();
//    dataObj.insert("messageId", strId);
//    dataObj.insert("type", "getUserReq");
    QString oldPhotoName = sqlDatabase->sqlSelectPhotoName(id);
    dataObj.insert("photoName", oldPhotoName);
    dataObj.insert("sn", switchCtl->m_sn);
    dataObj.insert("mid", id);

    WriteDataToServer(DEV_PERSON_REQUEST,dataObj);
}

void V1TcpClient::requestGetAllUserID()
{
    QJsonObject dataObj;
    dataObj.insert("sn", switchCtl->m_sn);
    qt_debug() << "requestGetAllUserID" << dataObj;
    WriteDataToServer(DEV_ALL_PERSON_ID_REQUEST,dataObj);
}

int V1TcpClient::getTimeZone()
{
    int hourNum = 0;
    QString timeZoneStr = switchCtl->m_timeZone;
    if(timeZoneStr.contains("(UTC"))
    {
        QString hour = timeZoneStr.right(7).mid(1,5);
        if(timeZoneStr.right(7).at(0) == "-")
        {
           hourNum = 0 - hour.mid(0,2).toInt();
        }
        else if(timeZoneStr.right(7).at(0) == "+")
        {
            hourNum = hour.mid(0,2).toInt();
        }
//        qt_debug() << hour << hourNum;
    }
    return hourNum;
}

int V1TcpClient::getTimeZoneMin()
{
    int minuteNum = 0;
    QString timeZoneStr = switchCtl->m_timeZone;
    if(timeZoneStr.contains("(UTC"))
    {
        QString timeStr = timeZoneStr.right(7).mid(1,5);
        if(timeZoneStr.right(7).at(0) == "-")
        {
           minuteNum = 0 - timeStr.mid(3,2).toInt();
        }
        else if(timeZoneStr.right(7).at(0) == "+")
        {
            minuteNum = timeStr.mid(3,2).toInt();
        }
//        qt_debug() << hour << hourNum;
    }
    return minuteNum;
}

void V1TcpClient::parseRegister(const QJsonObject &rootObj)
{
    if(rootObj.contains("result"))
    {
       int res = rootObj.value("result").toInt();
       qDebug()<<__PRETTY_FUNCTION__<<__LINE__<<res;
    }
    if(rootObj.contains("data"))
    {
        QJsonValue dataValue = rootObj.value("data");
        if(dataValue.isObject())
        {
            QString passwd;
            QJsonObject dataObj = dataValue.toObject();
            if(dataObj.contains("passwd"))
            {
                passwd = dataObj.value("passwd").toString();
                /*加签秘钥，登录时需要用到*/
                switchCtl->m_passwd = passwd;
                switchCtl->saveSwitchParam();
                requestDoortoken();
            }
        }
    }
}

void V1TcpClient::parseDoortoken(const QJsonObject &rootObj)
{
    qt_debug() << "doorToken:" << rootObj;
    if(rootObj.contains("result"))
    {
        int result = rootObj.value("result").toInt();
        if (1002 == result){
            requestRegister();
        } else if (200 == result) {
            dataShare->m_netStatus = true;
            requestGetAllUserID();
        }
    }
}

void V1TcpClient::parseHeartbeat(const QJsonObject &rootObj)
{
    if(rootObj.contains("result"))
    {
        int result = rootObj.value("result").toInt();
        qt_debug() << result;
        if (401 == result) {
            Reconnect();
        }
    }
}

void V1TcpClient::parseGetUsers(const QJsonObject &rootObj)
{
    if(rootObj.contains("result")) {
        int result = rootObj.value("result").toInt();
        qt_debug() << result;
    }
    if(rootObj.contains("messageId"))
    {
        QString uuid = rootObj.value("messageId").toString();
        qt_debug() << uuid;
    }
    if(rootObj.contains("data")) {
        QJsonValue dataValue = rootObj.value("data");
        if(dataValue.isObject()) {
            emit updateUsers(dataValue.toObject());
        }
    }
}

void V1TcpClient::parseRecord(bool isPhoto, const QJsonObject &rootObj)
{
    qt_debug() << rootObj;
    if (200 == rootObj.value("result").toInt()) {
        if(!rootObj.contains("messageId"))
        {
            return;
        }
        int id = rootObj.value("messageId").toInt();
        qDebug() << "delete offline data id:" << id;
        QString fileImg1 = "rm offline/" + QString::number(id) + ".jpg";
        system(fileImg1.toStdString().c_str());
        sqlDatabase->sqlDeleteOffline(id);
    }
}

void V1TcpClient::parseInsertFail(const QJsonObject &rootObj)
{
    if (200 == rootObj.value("result").toInt())
    {
        sqlDatabase->sqlDeleteAllFail();
    }
}

void V1TcpClient::parseGetAllIC(const QJsonObject &rootObj)
{
    qt_debug() << rootObj;
    if (200 == rootObj.value("result").toInt()) {
        emit allIC(rootObj["data"].toArray());
    } else {
        qt_debug() << "parseGetAllIC ERROR !";
    }
}

void V1TcpClient::parseUploadAlarm(const QJsonObject &rootObj)
{
    qt_debug() << rootObj;
    if (200 == rootObj.value("result").toInt()) {
        qt_debug() << "parseUploadAlarm SUCCESS !";
    } else {
        qt_debug() << "parseUploadAlarm ERROR !";
    }
}

void V1TcpClient::uploadOffine(int id, int userId, const QString &photo, int isOver,int type, int isTemp, int sex, const QStringList &datas)
{
    qt_debug() << "uploadOffine id" << id << type;
    if(4 == type)
    {
        return;
    }
    QJsonObject jsonObj, obj;
    jsonObj.insert("sn", switchCtl->m_sn);
    jsonObj.insert("did", id);
    jsonObj.insert("messageId", id);
    jsonObj.insert("message", "offlineSnapshotReq");
    jsonObj.insert("userId", userId);
    jsonObj.insert("deviceId", 0);
    jsonObj.insert("unlockType", type);
    jsonObj.insert("photo", photo);
    jsonObj.insert("tempTime", datas.at(0));
    jsonObj.insert("unlockTime", datas.at(0));
    int sec = getTimeZoneMin()*60 + getTimeZone()*3600;
    jsonObj.insert("timeZone", switchCtl->m_timeZone);
    jsonObj.insert("zoneUnlockTime", QString("%1").arg(QDateTime::currentDateTime().addSecs(sec).toString("yyyy-MM-dd HH:mm:ss")));
    jsonObj.insert("temperature", datas.at(1));
    jsonObj.insert("isOver", isOver);
    jsonObj.insert("isTemp", isTemp);
    jsonObj.insert("isSuccess", datas.at(2).toInt());
    jsonObj.insert("invalidReason", datas.at(3));
    jsonObj.insert("isStranger", datas.at(4).toInt());
    WriteDataToServer(DEV_RECORD_PHOTO_REQUEST, jsonObj);
}

void V1TcpClient::uploadopenlog(int id, int userId, const QString &photo, int isOver,int type, int isTemp, int sex, const QStringList &datas)
{
    qt_debug() << "uploadopenlog" << datas;
    QJsonObject jsonObj;
    jsonObj.insert("sn", switchCtl->m_sn);
    jsonObj.insert("messageId", id);
    jsonObj.insert("message", "snapshotFaceReq");
    jsonObj.insert("userId", userId);
    jsonObj.insert("deviceId", 0);
    jsonObj.insert("unlockType", type);
    jsonObj.insert("photo", photo);
    jsonObj.insert("tempTime", datas.at(0));
    jsonObj.insert("unlockTime", datas.at(0));
    int sec = getTimeZoneMin()*60 + getTimeZone()*3600;
    jsonObj.insert("timeZone", switchCtl->m_timeZone);
    jsonObj.insert("zoneUnlockTime", QString("%1").arg(QDateTime::currentDateTime().addSecs(sec).toString("yyyy-MM-dd HH:mm:ss")));
    jsonObj.insert("temperature", datas.at(1));
    jsonObj.insert("isOver", isOver);
    jsonObj.insert("isTemp", isTemp);
    jsonObj.insert("isSuccess", datas.at(2).toInt());
    jsonObj.insert("invalidReason", datas.at(3));
    jsonObj.insert("isStranger", datas.at(4).toInt());
    jsonObj.insert("mjkh", datas.at(5));
    jsonObj.insert("realName", datas.at(6));
    jsonObj.insert("sex", sex);
    jsonObj.insert("cardNum", datas.at(7));
    jsonObj.insert("nation", datas.at(8));
    jsonObj.insert("address", datas.at(9));
    jsonObj.insert("birthday", datas.at(10));
    WriteDataToServer(DEV_DOOR_RECORD_REQUEST, jsonObj);
}

void V1TcpClient::parseGetCmd(QByteArray msgBody)
{
//    qt_debug() << msgBody;
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(msgBody, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qt_debug() << "jsonError.error" << jsonError.error;
        return;
    }
    QJsonObject jsonObj = document.object();
    if(jsonObj.contains("cmd"))
    {
        switch (jsonObj["cmd"].toInt()) {
            case 13:
            {
                responseServerSetup();
                break;
            }
            case 17:
            {
                QJsonObject data = jsonObj["data"].toObject();
                int mode = data["mode"].toInt();
                if(2 == mode)
                {
                    requestGetAllIC();
                }
                break;
            }
            default:
                break;
        }
    }
}

void V1TcpClient::responseServerSetup()
{
//    responseTimer->start();
    QJsonObject jsonObj;
    jsonObj.insert("result", 200);
    jsonObj.insert("desc", "Success");
    QJsonObject jsonData;
    jsonData.insert("name", switchCtl->m_devName);
    jsonData.insert("isTemp", switchCtl->m_tempCtl ? 1 : 0);
    jsonData.insert("isEg", switchCtl->m_faceDoorCtl ? 1 : 0);
    jsonData.insert("isIr", switchCtl->m_ir ? 1 : 0);
    jsonData.insert("isVi", switchCtl->m_vi ? 1 : 0);
    jsonData.insert("tempThreshold", switchCtl->m_warnValue);
    jsonData.insert("tempComp", switchCtl->m_tempComp);
    jsonData.insert("faceFeaturePairNumber", dataShare->m_faceThreshold);
    jsonData.insert("identifyWaitTime", switchCtl->m_identifyWaitTime);
    jsonData.insert("volume", switchCtl->m_volume);
    jsonData.insert("doorDelayTime", switchCtl->m_doorDelayTime);
    jsonData.insert("isFahrenheit", switchCtl->m_fahrenheit ? 1 : 0);
    jsonData.insert("isUploadStrangerAccess", switchCtl->m_uploadStrangerCtl ? 1 : 0);
    jsonData.insert("isUploadUserWithPhoto", switchCtl->m_uploadImageCtl ? 1 : 0);
    jsonData.insert("isMaskRecognition", switchCtl->m_mask);
    jsonObj.insert("data", jsonData);
    //WriteDataToServer(SERVER_RESPONSE_CMD, jsonObj);
    QByteArray data;
    QByteArray sendData;
    QJsonDocument document;
    document.setObject(jsonObj);
    data = document.toJson(QJsonDocument::Compact);
//    qt_debug() << data;
    sendData.clear();
    sendData.append(PACKET_HEAD);
    sendData.append(uchar(1));
    sendData.append(uchar(1));
    sendData.append(uchar(SERVER_RESPONSE_CMD));
    sendData.append(intToByte(data.length()));
    sendData.append(data);

    int backT = m_tcpSocket->write(sendData);  //发送数据到服务端
    qt_debug() << backT;
    //以下是为了防止发送数据的同时读取数据出现问题
    m_tcpSocket->flush();
}


void V1TcpClient::parseGetAllUserID(const QJsonObject &rootObj)
{
//    qDebug() << rootObj;
    if (200 == rootObj.value("result").toInt()) {
        emit allUserId(rootObj["data"].toArray());
    } else {

    }
}

void V1TcpClient::parseUploadopenlog(const QJsonObject &rootObj)
{
    qt_debug() << rootObj;
    if(rootObj.contains("result")) {
        int result = rootObj.value("result").toInt();
        if(200 == result)
        {
            if(rootObj.contains("messageId"))
            {
                int messageId = rootObj.value("messageId").toInt();
                QString fileImg1 = "rm offline/" + QString::number(messageId) + ".jpg";
                system(fileImg1.toStdString().c_str());
                sqlDatabase->sqlDeleteOffline(messageId);
            }
        }
    }
}
