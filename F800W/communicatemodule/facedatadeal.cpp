#include "facedatadeal.h"

FaceDataDeal::FaceDataDeal()
{
    m_status = false;
}

void FaceDataDeal::setPacket(FaceDataList *packet)
{
    m_packet = packet;
}

void FaceDataDeal::setHttp(HttpsClient *httpClient)
{
    m_httpsClient = httpClient;
}

void FaceDataDeal::dealJsonData(QJsonObject jsonObj)
{
    qDebug() << (MqttClient::MqttCmd)jsonObj["cmd"].toInt() << jsonObj;
    switch (jsonObj["cmd"].toInt())
    {
    case MqttClient::UserData:
    {
        dealFaceNewData(jsonObj);
        break;
    }
    case MqttClient::OpenDoor:
    {
        if(jsonObj.contains("data"))
        {
            QJsonObject jsonData = jsonObj["data"].toObject();
            if(jsonData.contains("id"))
            {

            }
        }
        break;
    }
    case MqttClient::DeviceUpdate:
    {
        break;
    }
    case MqttClient::UploadLog:
    {

        break;
    }
    case MqttClient::Unbind:
    {
        break;
    }
    case MqttClient::ChangeSetting:
    {
        break;
    }
    case MqttClient::Reboot:
    {
        system("reboot");
        break;
    }
    case MqttClient::ServerSyncEnd:

        break;
    case MqttClient::IdentifyRecord:
    {

        break;
    }
    case MqttClient::SetTcpServer:
    {

        break;
    }
    case MqttClient::ServerTime:
    {
        QJsonObject jsonData = jsonObj["data"].toObject();
        QString timeZone = jsonData["timeZone"].toString();
        break;
    }
    case MqttClient::FactorySetup:
    {

        system("rm faceDatas.db");
        system("rm offline/*");
        system("sync");
        system("reboot");
        break;
    }
    case MqttClient::ClearOfflineData:
    {

        break;
    }
    case MqttClient::ClearFailFace:
    {

        break;
    }
    default:
        break;
    }
}

void FaceDataDeal::dealFaceNewData(QJsonObject jsonObj)
{
    QJsonObject jsonData = jsonObj["data"].toObject();
    int id = jsonData["mid"].toInt();
    QString edittime = "", name = "", photo = "";
    QStringList text;
    text.clear();
    int passNum = -1, isBlack = -1;
    QVariantList users, auths;
    bool status = true;
    int cmd = jsonObj["operater"].toInt();
    if(2 == cmd)
    {
        users = sqlDatabase->sqlSelect(id);
        auths = sqlDatabase->sqlSelectAuth(id);
        if(auths.isEmpty())
        {
            status = false;
        }
    }
    QString remark = "", startTime = "", expireTime = "", passTimeSection = "", passPeriod = "", mobile = "", photoName = "";
    if(jsonData.contains("updateDate"))
    {
        edittime = jsonData["updateDate"].toString();
    }
    if(jsonData.contains("username"))
    {
        name = jsonData["username"].toString();
    }
    else {
        if(2 == cmd && !users.isEmpty())
        {
            name = users.at(1).toString();
        }
    }
    if(jsonData.contains("mobile"))
    {
        mobile = jsonData["mobile"].toString();
    }
    else {
        if(2 == cmd && !users.isEmpty())
        {
            mobile = users.at(5).toString();
        }
    }
    if(jsonData.contains("photoName"))
    {
        photoName = jsonData["photoName"].toString();
    }
    else {
        if(2 == cmd && !users.isEmpty())
        {
            photoName = users.at(4).toString();
        }
    }
    if(jsonData.contains("photoUrl"))
    {
        photo = jsonData["photoUrl"].toString();
    }
    if(jsonData.contains("passNum"))
    {
        passNum = jsonData["passNum"].toInt();
    }
    else {
        if(2 == cmd && status)
        {
            passNum = auths.at(1).toInt();
        }
    }
    if(jsonData.contains("startTime"))
    {
        startTime = jsonData["startTime"].toString();
    }
    else {
        if(2 == cmd && status)
        {
            startTime = auths.at(2).toString();
        }
    }
    if(jsonData.contains("expireTime"))
    {
        expireTime = jsonData["expireTime"].toString();
    }
    else {
        if(2 == cmd && status)
        {
            expireTime = auths.at(3).toString();
        }
    }
    if(jsonData.contains("isBlack"))
    {
        isBlack = jsonData["isBlack"].toInt();
    }
    else {
        if(2 == cmd && status)
        {
            isBlack = auths.at(4).toInt();
        }
    }
    if(jsonData.contains("passPeriod"))
    {
        passPeriod = jsonData["passPeriod"].toString();
    }
    else {
        if(2 == cmd && status)
        {
            passPeriod = auths.at(5).toString();
        }
    }
    if(jsonData.contains("passTimeSection"))
    {
        passTimeSection = jsonData["passTimeSection"].toString();
    }
    else {
        if(2 == cmd && status)
        {
            passTimeSection = auths.at(6).toString();
        }
    }
    if(jsonData.contains("remark"))
    {
        remark = jsonData["remark"].toString();
    }
    else {
        if(2 == cmd && status)
        {
            remark = auths.at(7).toString();
        }
    }
    if(jsonData.contains("mjkh"))
    {
        QString cardNo = jsonData["mjkh"].toString();
        sqlDatabase->sqlDeleteIc(id);
        if(!cardNo.isEmpty())
        {
            cardNo = cardNo.toLower();
            sqlDatabase->sqlInsertIc(id, cardNo);
        }
    }
    text << startTime << expireTime << passPeriod << passTimeSection << mobile << photoName << remark;
    switch (cmd) {
    case 1:
    {
        m_httpsClient->httpsRequestImage(id, photo);
        emit insertFaceGroups(id, name, edittime, photoName, mobile);
        sqlDatabase->sqlInsertAuth(id, passNum, isBlack, text);
        break;
    }
    case 2:
    {
        if(!photo.isEmpty())
        {
            sqlDatabase->sqlDelete(id);
            sqlDatabase->sqlDeleteAuth(id);
            m_httpsClient->httpsRequestImage(id, photo);
            emit insertFaceGroups(id, name, edittime, photoName, mobile);
            sqlDatabase->sqlInsertAuth(id, passNum, isBlack, text);
        }
        else {
            sqlDatabase->sqlUpdate(id, name, edittime, photoName, mobile);
            sqlDatabase->sqlUpdateAuth(id, passNum, isBlack, text);
        }
        break;
    }
    case 3:
    {
        sqlDatabase->sqlDelete(id);
        sqlDatabase->sqlDeleteAuth(id);
        sqlDatabase->sqlDeleteIc(id);
        break;
    }
    default:
        break;
    }
}

void FaceDataDeal::run()
{
    while(true)
    {
        FacePacketNode_t *data = m_packet->GetLogPacket();
        if(data)
        {
            QJsonParseError jsonError;
            QJsonDocument document = QJsonDocument::fromJson(data->datas, &jsonError);
            if (jsonError.error != QJsonParseError::NoError) {
                qDebug() << "jsonError.error" << jsonError.error;
                return;
            }
            QJsonObject jsonObj = document.object();
            dealJsonData(jsonObj);
            delete data;
            data = nullptr;
        }
        else {
           sleep(1);
        }
    }
}
