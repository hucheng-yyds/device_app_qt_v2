#include <QFile>
#include "useridrequest.h"
#include "datashare.h"

UserIdRequest::UserIdRequest()
{
    m_faceSyncStatus = false;
    m_startFaceDownload = false;
}

void UserIdRequest::updateIdentifyValue()
{
    int count = sqlDatabase->m_localFaceSet.size();
    if (0 == count || count < 1000)
    {
        dataShare->m_faceThreshold = 61;
    }
    else if (1000 <= count && count < 5000)
    {
        dataShare->m_faceThreshold = 65;
    }
    else if (5000 <= count && count < 10000)
    {
        dataShare->m_faceThreshold = 66;
    }
    else if (10000 <= count && count < 20000)
    {
        dataShare->m_faceThreshold = 68;
    }
    else if (20000 <= count && count < 50000)
    {
        dataShare->m_faceThreshold = 70;
    }
    else if (50000 <= count && count < 100000)
    {
        dataShare->m_faceThreshold = 71;
    }
}

void UserIdRequest::run()
{
    int count = 0;
    while(true)
    {
        if(dataShare->m_netStatus && m_startFaceDownload)
        {
            int size = m_updateFace.size();
            if(size > 0)
            {
                if(m_faceSyncStatus)
                {
                    dataShare->m_sync = true;
                    count = 0;
                    m_faceSyncStatus = false;
                    m_curFaceId = *m_updateFace.begin();
                    m_updateFace.remove(m_curFaceId);
                    qDebug() << "getUser" << "id" << m_curFaceId << "counts" << size;
                    emit getUsers(m_curFaceId);
                }
            }
            else
            {
                emit allUserIc();
                sleep(3);
//                updateIdentifyValue();
                dataShare->m_sync = false;
                m_startFaceDownload = false;
            }
            if(!m_faceSyncStatus)
            {
                count++;
                if(count > 20 * switchCtl->m_tcpTimeout)
                {
                    if(m_updateFace.size() > 0)
                    {
                        sqlDatabase->sqlInsertFail(m_curFaceId, 9);
                    }
                    count = 0;
                    m_faceSyncStatus = true;
                }
            }
            msleep(50);
        }
        else {
            if(!dataShare->m_netStatus && m_startFaceDownload)
            {
                dataShare->m_sync = false;
                m_startFaceDownload = false;
                m_updateFace.clear();
            }
            sleep(1);
        }
    }
}

void UserIdRequest::httpsUpdateUsers(const QJsonObject &jsonObj)
{
    QJsonObject jsonData = jsonObj["users"].toObject();
    QString startTime = "", expireTime = "", passTimeSection = "", passPeriod = "", mobile = "", photoName = "", cardNo = "", remark = "";
    QStringList text;
    text.clear();
    int passNum = -1, isBlack = -1;
    int id = jsonData["mid"].toInt();
    m_updateFace.remove(id);
    QString name = jsonObj["username"].toString();
    QString photo = jsonObj["photo"].toString();
    QString edittime = jsonObj["updateDate"].toString();
    if(jsonObj.contains("passNum"))
    {
        passNum = jsonObj["passNum"].toInt();
    }
    if(jsonObj.contains("startTime"))
    {
        startTime = jsonObj["startTime"].toString();
    }
    if(jsonObj.contains("expireTime"))
    {
        expireTime = jsonObj["expireTime"].toString();
    }
    if(jsonObj.contains("isBlack"))
    {
        isBlack = jsonObj["isBlack"].toInt();
    }
    if(jsonObj.contains("passPeriod"))
    {
        passPeriod = jsonObj["passPeriod"].toString();
    }
    if(jsonObj.contains("passTimeSection"))
    {
        passTimeSection = jsonObj["passTimeSection"].toString();
    }
    if(jsonObj.contains("mobile"))
    {
        mobile = jsonObj["mobile"].toString();
    }
    if(jsonObj.contains("photoName"))
    {
        photoName = jsonObj["photoName"].toString();
    }
    if(jsonObj.contains("remark"))
    {
        remark = jsonObj["remark"].toString();
    }
    if(jsonObj.contains("mjkh"))
    {
        cardNo = jsonObj["mjkh"].toString();
        if(!cardNo.isEmpty())
        {
            cardNo = cardNo.toLower();
            sqlDatabase->sqlInsertIc(id, cardNo);
        }
        else
        {
            sqlDatabase->sqlDeleteIc(id);
        }
    }
    text << startTime << expireTime << passPeriod << passTimeSection << remark;
    sqlDatabase->sqlInsertAuth(id, passNum, isBlack, text);
    emit insertFaceGroups(id, name, edittime, photoName, mobile);
}

void UserIdRequest::tcpUpdateUsers(const QJsonObject &jsonObj)
{
    QString edittime = "", remark = "", startTime = "", expireTime = "", passTimeSection = "", passPeriod = "", mobile = "", photoName = "";
    int passNum = -1, isBlack = -1;
    int id = jsonObj["mid"].toInt();
//    QFile file1(QString("./%1.json").arg(id));
//    if(!file1.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
//        qDebug() << "File open failed!";
//    } else {
//        qDebug() <<"File open successfully!";
//    }
//    QJsonDocument jdoc(jsonObj);
//    file1.seek(0);
//    file1.write(jdoc.toJson());
//    file1.flush();
//    file1.close();
    QString name = "";
    QString photo = jsonObj["photo"].toString();
    bool status = false;
    QVariantList value;
    value.clear();
    value = sqlDatabase->sqlSelect(id);
    if(value.size() > 0)
    {
        status = true;
    }
    if(jsonObj.contains("updateDate"))
    {
        edittime = jsonObj["updateDate"].toString();
    }
    else {
        if(status)
        {
            edittime = value.at(2).toString();
        }
    }
    if(jsonObj.contains("username"))
    {
        name = jsonObj["username"].toString();
    }
    else {
        if(status)
        {
            name = value.at(1).toString();
        }
    }
    if(jsonObj.contains("mobile"))
    {
        mobile = jsonObj["mobile"].toString();
    }
    else {
        if(status)
        {
            mobile = value.at(5).toString();
        }
    }
    if(jsonObj.contains("photoName"))
    {
        photoName = jsonObj["photoName"].toString();
    }
    else {
        if(status)
        {
            photoName = value.at(4).toString();
        }
    }
    QFile file(QString::number(id) + ".jpg");
    file.open(QIODevice::ReadWrite);
    file.write(QByteArray::fromBase64(photo.toUtf8()));
    file.close();
    if(value.size() > 0)
    {
        sqlDatabase->sqlUpdate(id, name, edittime, photoName, mobile);
    }
    else {
        emit insertFaceGroups(id, name, edittime, photoName, mobile);
    }
    status = false;
    value.clear();
    value = sqlDatabase->sqlSelectAuth(id);
    if(jsonObj.contains("passNum"))
    {
        passNum = jsonObj["passNum"].toInt();
    }
    else {
        if(status)
        {
            passNum = value.at(1).toInt();
        }
    }
    if(jsonObj.contains("startTime"))
    {
        startTime = jsonObj["startTime"].toString();
    }
    else {
        if(status)
        {
            startTime = value.at(2).toString();
        }
    }
    if(jsonObj.contains("expireTime"))
    {
        expireTime = jsonObj["expireTime"].toString();
    }
    else {
        if(status)
        {
            expireTime = value.at(3).toString();
        }
    }
    if(jsonObj.contains("isBlack"))
    {
        isBlack = jsonObj["isBlack"].toInt();
    }
    else {
        if(status)
        {
            isBlack = value.at(4).toInt();
        }
    }
    if(jsonObj.contains("passPeriod"))
    {
        passPeriod = jsonObj["passPeriod"].toString();
    }
    else {
        if(status)
        {
            passPeriod = value.at(5).toString();
        }
    }
    if(jsonObj.contains("passTimeSection"))
    {
        passTimeSection = jsonObj["passTimeSection"].toString();
    }
    else {
        if(status)
        {
            passTimeSection = value.at(6).toString();
        }
    }
    if(jsonObj.contains("remark"))
    {
        remark = jsonObj["remark"].toString();
    }
    else {
        if(status)
        {
            remark = value.at(7).toString();
        }
    }
    QStringList datas;
    datas.clear();
    datas << startTime << expireTime << passPeriod << passTimeSection << remark;
    if(value.size() > 0)
    {
        sqlDatabase->sqlUpdateAuth(id, passNum, isBlack, datas);
    }
    else {
        sqlDatabase->sqlInsertAuth(id, passNum, isBlack, datas);
    }
    m_faceSyncStatus = true;
    m_updateFace.remove(id);
}

void UserIdRequest::onUpdateUsers(const QJsonObject &jsonObj)
{
    if (jsonObj.isEmpty())
    {
        qDebug() << "user is nonexistent !";
        sqlDatabase->sqlInsertFail(m_curFaceId, 9);
    }
    else
    {
        if(switchCtl->m_protocol != 3)
        {
            tcpUpdateUsers(jsonObj);
        }
        else
        {
            httpsUpdateUsers(jsonObj);
        }
    }
}

void UserIdRequest::onAlluserId(const QJsonArray &jsonArr)
{
    m_mutex.lock();
    QJsonArray faceJson = jsonArr;
    QSet<int> localFaceSet = sqlDatabase->m_localFaceSet;
    int size = faceJson.count();
    qDebug() << "OnAllUserId count:" << size << localFaceSet.size();
    m_updateFace.clear();
    if(size > 0)
    {
        foreach (QJsonValue val, faceJson)
        {
            QJsonObject jsonObj = val.toObject();
            int id = jsonObj["mid"].toInt();
            if(localFaceSet.contains(id))
            {
                QString newTime = jsonObj["updateDate"].toString();
                QString updateTime = sqlDatabase->sqlSelectAllUserTime(id);
                if(updateTime.compare(newTime) != 0)
                {
                    emit removeFaceGroup(id);
                    sqlDatabase->sqlDelete(id);
                    m_updateFace.insert(id);
                }
                localFaceSet.remove(id);
            }
            else
            {
                m_updateFace.insert(id);
            }
        }
        foreach(int i, localFaceSet)
        {
            sqlDatabase->sqlDelete(i);
        }
    }
    else {
        sqlDatabase->sqlDeleteAll();
        sqlDatabase->sqlDeleteAllIc();
        sqlDatabase->sqlDeleteAllAuth();
    }
    int count = m_updateFace.size();
    if(count > 0)
    {
        m_startFaceDownload = true;
        m_faceSyncStatus = true;
    }
    qDebug() << m_updateFace.size();
    m_mutex.unlock();
}

void UserIdRequest::onAllUsersIc(const QJsonArray &jsonArr)
{
    sqlDatabase->sqlDeleteAllIc();
    foreach(QJsonValue val, jsonArr)
    {
        QJsonObject obj = val.toObject();
        int mid = obj.value("mid").toInt();
        QString cardNo = obj.value("cardNo").toString();
        sqlDatabase->sqlInsertIc(mid, cardNo);
    }
    emit sigInsertFail();
}

void UserIdRequest::onNewUsers(const QJsonArray &jsonArr)
{
    foreach(QJsonValue val, jsonArr)
    {
        m_updateFace.insert(val.toInt());
    }
    if(m_updateFace.size() > 0)
    {
        m_startFaceDownload = true;
    }
}
