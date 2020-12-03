#include "serverdatadeal.h"
#include "datashare.h"

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

bool ServerDataDeal::checkVersion(const QString &target, const QString &current)
{
    QList<QString> tList = target.split(".");
    QList<QString> cList = current.split(".");
    if (tList.value(0).toInt() > cList.value(0).toInt()) {
        return true;
    } else if (tList.value(1).toInt() > cList.value(1).toInt()) {
        return true;
    } else if (tList.value(2).toInt() > cList.value(2).toInt()) {
        return true;
    } else if (tList.value(3).toInt() > cList.value(3).toInt()) {
        return true;
    } else if (tList.value(4).toInt() > cList.value(4).toInt()) {
        return true;
    }
    return false;
}

void ServerDataDeal::upgradeFile(const QJsonObject &obj)
{
    if (("1" == obj["deviceType"].toString() || DEVICE_TYPE == obj["deviceType"].toString()) &&
            checkVersion(obj["version"].toString(), VERSION))
    {
        system("rm update.tar.xz");
        m_httpsClient->httpsDownload(obj["downloadUrl"].toString());
        QFile file("update.tar.xz");
        if (!file.open(QFile::ReadWrite))
        {
            qt_debug() << "open failed!";
            return ;
        }
        QByteArray data = file.readAll();
        file.close();
        QString md5 = QCryptographicHash::hash(data, QCryptographicHash::Md5).toHex();
        if (md5 == obj["md5"].toString()) {
            system("tar -xvf update.tar.xz && rm update.tar.xz");
            if (QFile::exists("hi3516dv300_smp_image")) {
                char* mac = switchCtl->m_sn.toUtf8().data();
                if (mac[7] >= 'A') {
                    mac[7] -= '7';
                    mac[7] &= 0xE;
                    mac[7] += '7';
                } else {
                    mac[7] -= '0';
                    mac[7] &= 0xE;
                    mac[7] += '0';
                }
                system("echo " + QByteArray(mac) + " > /dev/mmcblk0p6");
                system("dd if=hi3516dv300_smp_image/u-boot-hi3516dv300.bin of=/dev/mmcblk0p1 &&"
                       "dd if=hi3516dv300_smp_image/uImage_hi3516dv300_smp of=/dev/mmcblk0p2 &&"
                       "rm hi3516dv300_smp_image -rf &&"
                       "sync");
            }
            if(QFile::exists("updateAlgorithm"))
            {
                system("rm updateAlgorithm ofzl.db ofzluser.db userdata.db ofzldata.db");
            }
            system("reboot");
        }
        else
        {
            system("rm update.tar.xz");
            qt_debug() << "md5 error !";
        }
    }
}

void ServerDataDeal::saveSetting(const QJsonObject &jsonData)
{
    qt_debug() << jsonData;
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
    qt_debug() << (MqttClient::MqttCmd)jsonObj["cmd"].toInt() << jsonObj.size();
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
                dataShare->m_offlineFlag = false;
//                hardware->ctlLed(GREEN);
                hardware->checkOpenDoor();
                QStringList datas;
                datas.clear();
                int offlineNmae = QDateTime::currentDateTime().toTime_t();
                int id = jsonData.value("id").toInt();
                datas << QDateTime::currentDateTime().addSecs(28800).toString("yyyy-MM-dd HH:mm:ss") << "" << "1" << "" << "0" << "" ;
                emit uploadopenlog(offlineNmae, id, "", 0, 2, 1, datas);
                sqlDatabase->sqlInsertOffline(offlineNmae, id, 2, 0, 0, datas);
            }
        }
        break;
    }
    case MqttClient::Bind:
    {
        system("rm *.db");
        emit allUserId();
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
        system("rm *.db");
        system("reboot");
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
        switchCtl->m_timeZone = timeZone;
        switchCtl->saveSwitchParam();
        break;
    }
    case MqttClient::IC:
    {
        QJsonObject obj = jsonObj["data"].toObject();
        dealIcNewData(obj);
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
        sqlDatabase->sqlDeleteAllOffline();
        break;
    }
    case MqttClient::ClearFailFace:
    {
        sqlDatabase->sqlDeleteAllFail();
        break;
    }
    default:
        break;
    }
    QString type = jsonObj.value("message").toString();
    QString messageId = jsonObj.value("messageId").toString();
    emit responseServer(type.replace("Req", "Rsp"), messageId, jsonData);
}

void ServerDataDeal::dealIcNewData(QJsonObject jsonObj)
{
    int mode = jsonObj.value("mode").toInt();
    if(1 == mode)
    {
        QJsonArray values = jsonObj.value("cardData").toArray();
        foreach(QJsonValue val, values)
        {
            QJsonObject obj = val.toObject();
            int cmd = obj.value("operater").toInt();
            int mid = obj.value("mid").toInt();
            if(1 == cmd)
            {
                sqlDatabase->sqlInsertIc(mid, obj.value("cardNo").toString());
            }
            else
            {
                sqlDatabase->sqlDeleteIc(mid);
            }
        }
    }
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
                dataShare->m_sync = true;
                sqlDatabase->sqlDelete(mid);
                sqlDatabase->sqlDeleteAuth(mid);
            }
            else {
                datas.push_front(mid);
            }
        }
        dataShare->m_sync = false;
        emit newUsers(datas);
    }
    else {
        qt_debug() << "dealFaceNewData:" << jsonObj;
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
                qt_debug() << "jsonError.error" << jsonError.error;
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
