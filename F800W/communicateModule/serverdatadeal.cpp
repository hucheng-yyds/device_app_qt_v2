#include "serverdatadeal.h"

ServerDataDeal::ServerDataDeal()
{
    m_status = false;
}

void ServerDataDeal::setPacket(ServerDataList *packet)
{
    m_packet = packet;
}

void ServerDataDeal::setHttp(HttpsClient *httpClient)
{
    m_httpsClient = httpClient;
}

void ServerDataDeal::upgradeFile(const QJsonObject &obj)
{

}

void ServerDataDeal::saveSetting(const QJsonObject &jsonData)
{
    qDebug() << "32234232:" << jsonData;
    if (jsonData.contains("isTemp")) {
        if(jsonData["isTemp"].toInt())
        {
            switchCtl->m_tempCtl = true;
        }
        else {
            switchCtl->m_tempCtl = false;
        }
        if (!jsonData["isTemp"].toInt()) {
            switchCtl->m_openMode = "Face";
        }
    }
    if (jsonData.contains("isEg")) {
        if(jsonData["isEg"].toInt())
        {
            switchCtl->m_faceDoorCtl = true;
        }
        else {
            switchCtl->m_faceDoorCtl = false;
        }
        if (!jsonData["isEg"].toInt())
        {
            switchCtl->m_openMode = "Temp";
        }
    }
    if(jsonData.contains("openDoorType"))
    {
        int type = jsonData["openDoorType"].toInt();
        if(1 == type)
        {
            switchCtl->m_openMode = "Face";
        }
        else if(2 == type)
        {
            switchCtl->m_openMode = "Temp";
        }
        else if(3 == type)
        {
            switchCtl->m_openMode = "FaceTemp";
        }
    }
    if (jsonData.contains("isIr"))
    {
        if(jsonData["isIr"].toInt())
        {
            switchCtl->m_ir = true;
        }
        else {
            switchCtl->m_ir = false;
        }
    }
    if(jsonData.contains("recoDistance"))
    {
        int distance = jsonData["recoDistance"].toInt();
        int len = 0;
        if(0 == distance)
        {
            len = 125;
        }
        else if(0 == distance)
        {
            len = 80;
        }
        else if(0 == distance)
        {
            len = 45;
        }
        else {
            len = 0;
        }
        switchCtl->m_identifyDistance = len;
    }
    if (jsonData.contains("isVi"))
    {
        if(jsonData["isVi"].toInt())
        {
            switchCtl->m_vi = true;
        }
        else {
            switchCtl->m_vi = false;
        }
    }
    if (jsonData.contains("tempThreshold"))
    {
        switchCtl->m_warnValue = jsonData["tempThreshold"].toDouble();
    }
    if (jsonData.contains("tempComp"))
    {
        switchCtl->m_tempComp =  jsonData["tempComp"].toDouble();
    }
    if(jsonData.contains("hardHatRecognition"))
    {
        if(jsonData.value("hardHatRecognition").toInt())
        {
            switchCtl->m_helet = true;
        }
        else {
            switchCtl->m_helet = false;
        }
    }
    if (jsonData.contains("identifyWaitTime"))
    {
        switchCtl->m_identifyWaitTime = jsonData["identifyWaitTime"].toInt();
    }
    if(jsonData.contains("identifyIdCardValue"))
    {
        switchCtl->m_idcardValue = jsonData["identifyIdCardValue"].toDouble();
    }
    if(jsonData.contains("tempBroadcastForm"))
    {
        if(jsonData["tempBroadcastForm"].toInt())
        {
            switchCtl->m_tempValueBroadcast = true;
        }
        else {
            switchCtl->m_tempValueBroadcast = false;
        }
    }
    if (jsonData.contains("volume"))
    {
        int volume = jsonData["volume"].toInt();
        switchCtl->m_volume = volume;
        if(0 == volume)
        {
            volume = -106;
        }
        else {
            volume = volume / 5 - 14;
        }
        IF_Vol_Set(volume);
    }
    if (jsonData.contains("name"))
    {
        switchCtl->m_devName = jsonData["name"].toString();
    }
    if (jsonData.contains("doorDelayTime"))
    {
        switchCtl->m_doorDelayTime = jsonData["doorDelayTime"].toInt();
    }
    if(jsonData.contains("isFahrenheit"))
    {
        if(jsonData["isFahrenheit"].toInt())
        {
            switchCtl->m_fahrenheit = true;
        }
        else {
            switchCtl->m_fahrenheit = false;
        }
    }
    if(jsonData.contains("isUploadStrangerAccess"))
    {
        if(jsonData["isUploadStrangerAccess"].toInt())
        {
            switchCtl->m_uploadStrangerCtl = true;
        }
        else {
            switchCtl->m_uploadStrangerCtl = false;
        }
    }
    if(jsonData.contains("isUploadUserWithPhoto"))
    {
        if(jsonData["isUploadUserWithPhoto"].toInt())
        {
            switchCtl->m_uploadImageCtl = true;
        }
        else {
            switchCtl->m_uploadImageCtl = false;
        }
    }
    if(jsonData.contains("isMaskRecognition"))
    {
        switchCtl->m_mask = jsonData["isMaskRecognition"].toInt();
    }
    if (jsonData.contains("infraredOnTime"))
    {
        switchCtl->m_bgrLightCtl = jsonData["infraredOnTime"].toString();
    }
    if(jsonData.contains("lowPowerSwitch"))
    {
        if(jsonData.value("lowPowerSwitch").toInt())
        {
            switchCtl->m_screenCtl = true;
        }
        else {
            switchCtl->m_screenCtl = false;
        }
    }
    if(jsonData.contains("lowPowerTimes"))
    {
        switchCtl->m_closeScreenTime = jsonData.value("lowPowerTimes").toInt();
    }
    if(jsonData.contains("usernameMasking"))
    {

    }
    if(jsonData.contains("icCardShow"))
    {

    }
    switchCtl->saveSwitchParam();
    system("sync");
}

void ServerDataDeal::dealJsonData(QJsonObject jsonObj)
{
    QJsonObject jsonData;
    qDebug() << (MqttClient::MqttCmd)jsonObj["cmd"].toInt() << jsonObj;
    switch (jsonObj["cmd"].toInt())
    {
    case MqttClient::UserData: {
        QJsonObject jsonData = jsonObj["data"].toObject();
        dealFaceNewData(jsonData);
        break;
    }
    case MqttClient::OpenDoor:
    {
        if(jsonObj.contains("data"))
        {
            QJsonObject jsonData = jsonObj["data"].toObject();
            if(jsonData.contains("id"))
            {
                hardware->ctlLed(GREEN);
                hardware->checkOpenDoor();
                QStringList datas;
                datas.clear();
                int id = jsonData.value("id").toInt();
                datas << "m_tempVal" << "1" << "" << "0" << "" << QDateTime::currentDateTime().addSecs(28800).toString("yyyy-MM-dd HH:mm:ss");
                sqlDatabase->sqlInsertOffline(0, id, 1, 0, 0, datas);
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
        sqlDatabase->sqlDeleteAll();
        system("rm *.db");
        break;
    }
    case MqttClient::ChangeSetting:
    {
        QJsonObject obj = jsonObj["data"].toObject();
        saveSetting(obj);
        break;
    }
    case MqttClient::UpgradeBase64:
    {
        QJsonObject obj = jsonObj["data"].toObject();
        upgradeFile(obj);
        break;
    }
    case MqttClient::Reboot:
    {
        system("reboot");
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
        switchCtl->setSwitchDefault();
        system("rm *.db");
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
    emit responseServer(jsonData);
}

void ServerDataDeal::dealFaceNewData(QJsonObject jsonObj)
{
    if(jsonObj.contains("updatePerson"))
    {
        QJsonArray dataArr = jsonObj.value("updatePerson").toArray();
        QJsonArray datas;
        foreach(QJsonValue val, dataArr)
        {
            QJsonObject mids = val.toObject();
            int cmd = mids["operater"].toInt();
            int mid = mids["mid"].toInt();
            if(3 == cmd)
            {
                sqlDatabase->sqlDelete(mid);
                sqlDatabase->sqlDeleteAuth(mid);
            }
            else {
                datas.push_front(mid);
            }
        }
        emit newUsers(datas);
    }
    else {
        qDebug() << "dealFaceNewData:" << jsonObj;
    }
}

void ServerDataDeal::run()
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
           msleep(600);
        }
    }
}
