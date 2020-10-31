#include "tcpclient.h"

TcpClient::TcpClient()
{
    moveToThread(this);
}

void TcpClient::run()
{
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

void TcpClient::requestRegister()
{
    QDateTime current_date_time =QDateTime::currentDateTime().addSecs(28800);
    QString timestamp =current_date_time.toString("yyyyMMddhhmmss");
    QJsonObject dataObj;
    dataObj.insert("sn", switchCtl->m_sn);
    dataObj.insert("timestamp", timestamp.toInt());
    dataObj.insert("type", DEVICE_TYPE);
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
    QJsonObject dataObj;
    sign = hash.toHex();
    dataObj.insert("sn", sn);
    dataObj.insert("timestamp", timestamp.toInt());
    dataObj.insert("dversion", "1");
    dataObj.insert("sign", sign);

    WriteDataToServer(DEV_LOGIN_REQUEST, dataObj);
}

void TcpClient::requestHeartbeat()
{
    int count = sqlDatabase->m_localFaceSet.size();
    QJsonObject dataObj;
    dataObj.insert("sn", switchCtl->m_sn);
    dataObj.insert("peopleCount", count);
    dataObj.insert("capacity", 30000);
    dataObj.insert("appVersion", VERSION);
    dataObj.insert("networkName", "eth0");
    dataObj.insert("ipAddr", switchCtl->m_ipAddr);
    dataObj.insert("languageSet", switchCtl->m_language);

    WriteDataToServer(DEV_HEARTBEAT_REQUEST, dataObj);
}

void TcpClient::requestUsersChange()
{
    QJsonObject dataObj;
    dataObj.insert("sn", switchCtl->m_sn);
    qDebug() << "requestUsersChange" << dataObj;
    WriteDataToServer(DEV_ALL_PERSON_CHANGE_REQUEST, dataObj);
}

void TcpClient::requestGetAllUserID()
{
    QJsonObject dataObj;
    dataObj.insert("sn", switchCtl->m_sn);
    qDebug() << "requestGetAllUserID" << dataObj;
    WriteDataToServer(DEV_ALL_PERSON_ID_REQUEST,dataObj);
}

void TcpClient::requestGetUsers(int id)
{
    QJsonObject dataObj;
    QString oldPhotoName = "";
    dataObj.insert("photoName", oldPhotoName);
    dataObj.insert("sn", switchCtl->m_sn);
    dataObj.insert("mid", id);

    WriteDataToServer(DEV_PERSON_REQUEST,dataObj);
}

void TcpClient::requestGetAllUserAuth()
{
    QJsonObject dataObj;
    dataObj.insert("sn", switchCtl->m_sn);
    qDebug() << "requestGetAllUserAuth" << dataObj;
    WriteDataToServer(DEV_ALL_PERSON_AUTH_REQUEST,dataObj);
}

void TcpClient::requestGetAllUserIC()
{
    QJsonObject dataObj;
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
    sendData.append(uchar(1));
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
