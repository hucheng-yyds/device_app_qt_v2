#include "tcpmiddware.h"
#include "datashare.h"

#define TCPHEAD "OFZL"

TcpMiddware::TcpMiddware()
{
    moveToThread(this);
}

void TcpMiddware::run()
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
    connect(m_heartbeatTimer, &QTimer::timeout, this, &TcpMiddware::requestHeartbeat);
    connect(m_connectTimer, &QTimer::timeout, this, &TcpMiddware::Reconnect);
    connect(m_requestTimer, &QTimer::timeout, this, &TcpMiddware::Reconnect);
    ConnectHost();
    exec();
}

void TcpMiddware::setPacket(ServerDataList *packet)
{
    m_serverData = packet;
}

void TcpMiddware::ConnectHost()
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
        requestLogin();
    }
}

void TcpMiddware::Reconnect()
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

void TcpMiddware::ConnectError(QAbstractSocket::SocketError state)
{
    qt_debug()<<state;
    if(QAbstractSocket::RemoteHostClosedError == state)
    {

    }
}

void TcpMiddware::OnStateChanged(QAbstractSocket::SocketState state)
{
    qt_debug()<<state;
    if(state == QAbstractSocket::UnconnectedState)
    {
    }
}

void TcpMiddware::parseData(int cmdType, QByteArray &recData)
{
    QJsonParseError jsonError;
    QJsonDocument json = QJsonDocument::fromJson(recData, &jsonError);
    qt_debug() << jsonError.error << cmdType;
    if (jsonError.error == QJsonParseError::NoError)
    {
        if (json.isObject())
        {
            QJsonObject jsonObj = json.object();
            if(cmdType == DEV_LOGIN_RESPONSE)
            {
                qDebug() << jsonObj;
                pareLogin(jsonObj);
            }
            if(cmdType == DEV_HEARTBEAT_RESPONSE)
            {
                qDebug() << jsonObj;
                pareHeartbeat(jsonObj);
            }
            if(cmdType == DEV_DOOR_RECORD_RESPONSE)
            {
                parseUploadData(jsonObj);
            }
            if(cmdType == DEV_GET_ALL_IC_RESPONE)
            {
                parseAllIc(jsonObj);
            }
            if(cmdType == SERVER_REQUEST_CMD)
            {
                FacePacketNode_t *pack = new FacePacketNode_t;
                pack->datas = recData;
                m_serverData->PushLogPacket(pack);
                parseServerSeting(jsonObj);
            }
        }
    }
}

void TcpMiddware::checkReadData(QByteArray readData)
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

void TcpMiddware::OnReadData()
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
                qt_debug() << "data Error";
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

void TcpMiddware::pareLogin(const QJsonObject &jsonObj)
{
    int messageId = jsonObj.value("messageId").toString().toInt();
    int result = jsonObj.value("result").toInt();
    if(1 == messageId && 200 == result)
    {
        dataShare->m_netStatus = true;
        requestGetAllUserID();
    }
}

void TcpMiddware::pareHeartbeat(const QJsonObject &jsonObj)
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

void TcpMiddware::parseUploadData(const QJsonObject &jsonObj)
{
    int result = jsonObj.value("result").toInt();
    int messageId = jsonObj.value("messageId").toString().toInt();
    qt_debug() << "parseUploadData" << result << messageId;
    if(200 == result)
    {
        QString fileImg1 = "rm /mnt/UDISK/offline/" + QString::number(messageId) + ".jpg";
        system(fileImg1.toStdString().c_str());
        sqlDatabase->sqlDeleteOffline(messageId);
    }
}

void TcpMiddware::parseAllIc(const QJsonObject &jsonObj)
{
    int result = jsonObj.value("result").toInt();
    int messageId = jsonObj.value("messageId").toString().toInt();
    if(200 == result)
    {
        emit allUserIc(jsonObj["data"].toArray());
    }
    else {
        qt_debug() << "parseAllUserAuth result" << result << "messageId" << messageId;
    }
}

void TcpMiddware::parseServerSeting(const QJsonObject &jsonObj)
{
    if(jsonObj.contains("cmd"))
    {
        int cmd = jsonObj["cmd"].toInt();
        if(13 == cmd)
        {
            QString messageId = jsonObj.value("messageId").toString();
            responseServerSetup(messageId);
        }
        else if(17 == cmd)
        {
            QJsonObject data = jsonObj["data"].toObject();
            int mode = data["mode"].toInt();
            if(2 == mode)
            {
                requestGetAllUserIC();
            }
        }
    }
}

void TcpMiddware::requestInserFail()
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

void TcpMiddware::requestLogin()
{
    QString rec_passwd = switchCtl->m_passwd;
    QString sn = switchCtl->m_sn;
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time = QDateTime::currentDateTime().addSecs(28800);
    QString timestamp = QString("%1").arg(origin_time.secsTo(current_date_time));
    QString sign = sn + rec_passwd + timestamp + TCPHEAD;
    QByteArray hash = QCryptographicHash::hash(sign.toLatin1(), QCryptographicHash::Md5);
    QJsonObject dataObj, obj;
    sign = hash.toHex();
    dataObj.insert("sn", sn);
    dataObj.insert("messageId", "1");
    dataObj.insert("message", "loginReq");
    obj.insert("timestamp", timestamp.toInt());
    obj.insert("dversion", VERSION);
    obj.insert("protocolVersion", "2");
    obj.insert("sign", sign);
    dataObj.insert("data", obj);
    qt_debug() << dataObj;
    WriteDataToServer(DEV_LOGIN_REQUEST, dataObj);
}

void TcpMiddware::requestHeartbeat()
{
    int count = sqlDatabase->m_localFaceSet.size();
    QJsonObject dataObj, obj;
    dataObj.insert("messageId", QString("%1").arg(m_seq++));
    dataObj.insert("message", "heartbeatReq");
    dataObj.insert("sn", switchCtl->m_sn);
    obj.insert("peopleCount", count);
    obj.insert("capacity", 30000);
    obj.insert("appVersion", VERSION);
    obj.insert("networkName", "eth0");
    obj.insert("ipAddr", dataShare->m_ipAddr);
    obj.insert("languageSet", switchCtl->m_language);
    dataObj.insert("data", obj);
    WriteDataToServer(DEV_HEARTBEAT_REQUEST, dataObj);
}

void TcpMiddware::requestGetAllUserID()
{
    QJsonObject dataObj;
    dataObj.insert("messageId", "3");
    dataObj.insert("message", "allUserReq");
    dataObj.insert("sn", switchCtl->m_sn);
    qt_debug() << "requestGetAllUserID" << dataObj;
    WriteDataToServer(DEV_ALL_PERSON_ID_REQUEST,dataObj);
}

void TcpMiddware::requestGetUsers(int id)
{
    QJsonObject dataObj, obj;
    QString oldPhotoName = sqlDatabase->sqlSelectPhotoName(id);
    dataObj.insert("messageId", QString("%1").arg(id));
    dataObj.insert("message", "getUserReq");
    dataObj.insert("sn", switchCtl->m_sn);
    obj.insert("mid", id);
    obj.insert("photoName", oldPhotoName);
    dataObj.insert("data", obj);

    WriteDataToServer(DEV_PERSON_REQUEST,dataObj);
}

int TcpMiddware::getTimeZone()
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
    }
    return hourNum;
}

int TcpMiddware::getTimeZoneMin()
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

void TcpMiddware::uploadopenlog(int id, int userId, const QString &photo, int isOver,int type, int isTemp, int sex, const QStringList &datas)
{
    qt_debug() << "uploadopenlog" << datas;
    QJsonObject jsonObj, obj;
    obj.insert("sn", switchCtl->m_sn);
    obj.insert("messageId", QString("%1").arg(id));
    obj.insert("message", "getUserReq");
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
    obj.insert("data", jsonObj);
    WriteDataToServer(DEV_DOOR_RECORD_REQUEST, obj);
}

void TcpMiddware::responseServerSetup(const QString &messageId)
{
    QJsonObject jsonObj;
    jsonObj.insert("result", 200);
    jsonObj.insert("desc", "Success");
    jsonObj.insert("messageId", messageId);
    jsonObj.insert("message", "spGetConfigRsp");
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
    QByteArray data;
    QByteArray sendData;
    QJsonDocument document;
    document.setObject(jsonObj);
    data = document.toJson(QJsonDocument::Compact);
    sendData.clear();
    sendData.append(TCPHEAD);
    sendData.append(uchar(2));
    sendData.append(uchar(1));
    sendData.append(uchar(SERVER_RESPONSE_CMD));
    sendData.append(intToByte(data.length()));
    sendData.append(data);

    int backT = m_tcpSocket->write(sendData);  //发送数据到服务端
    qt_debug() << backT;
    //以下是为了防止发送数据的同时读取数据出现问题
    m_tcpSocket->flush();
}

void TcpMiddware::responseServer(const QString &type, const QString &messageId, const QJsonObject &jsonData)
{
    QJsonObject jsonObj;
    jsonObj.insert("result", 200);
    jsonObj.insert("desc", "Success");
    jsonObj.insert("messageId", messageId);
    jsonObj.insert("message", type);
    jsonObj.insert("data", jsonData);
    QByteArray data;
    QByteArray sendData;
    QJsonDocument document;
    document.setObject(jsonObj);
    data = document.toJson(QJsonDocument::Compact);
    sendData.clear();
    sendData.append(TCPHEAD);
    sendData.append(uchar(2));
    sendData.append(uchar(1));
    sendData.append(uchar(SERVER_RESPONSE_CMD));
    sendData.append(intToByte(data.length()));
    sendData.append(data);

    int backT = m_tcpSocket->write(sendData);  //发送数据到服务端
    qt_debug() << backT;
    //以下是为了防止发送数据的同时读取数据出现问题
    m_tcpSocket->flush();
}

void TcpMiddware::requestGetAllUserIC()
{
    QJsonObject dataObj;
    dataObj.insert("messageId", QString("%1").arg(m_seq++));
    dataObj.insert("message", "icReq");
    dataObj.insert("sn", switchCtl->m_sn);
    qt_debug() << "requestGetAllUserIC" << dataObj;
    WriteDataToServer(DEV_GET_ALL_IC_REQUEST,dataObj);
}

int TcpMiddware::bytesToInt(QByteArray bytes)
{
    int addr = bytes[0] & 0x000000FF;
    addr |= ((bytes[1] << 8) & 0x0000FF00);
    addr |= ((bytes[2] << 16) & 0x00FF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
}

QByteArray TcpMiddware::intToByte(int num)
{
    QByteArray abyte0;
    abyte0.resize(4);
    abyte0[0] = (uchar) ((0xff000000 & num) >> 24);
    abyte0[1] = (uchar) ((0x00ff0000 & num) >> 16);
    abyte0[2] = (uchar) ((0x0000ff00 & num) >> 8);
    abyte0[3] = (uchar) (0x000000ff & num);
    return abyte0;
}

void TcpMiddware::WriteDataToServer(int msgType, QJsonObject &postObj)
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
    sendData.clear();
    sendData.append(TCPHEAD);
    sendData.append(uchar(2));
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
