#include <QFile>
#include "useridrequest.h"

UserIdRequest::UserIdRequest()
{
    m_startFaceDownload = false;
    m_updateFaceStatus = false;
}

void UserIdRequest::updateIdentifyValue()
{
    size_t count = sqlDatabase->m_localFaceSet.size();
    if (0 == count || count < 1000)
    {
        switchCtl->m_faceThreshold = 61;
    }
    else if (1000 <= count && count < 5000)
    {
        switchCtl->m_faceThreshold = 65;
    }
    else if (5000 <= count && count < 10000)
    {
        switchCtl->m_faceThreshold = 66;
    }
    else if (10000 <= count && count < 20000)
    {
        switchCtl->m_faceThreshold = 68;
    }
    else if (20000 <= count && count < 50000)
    {
        switchCtl->m_faceThreshold = 70;
    }
    else if (50000 <= count && count < 100000)
    {
        switchCtl->m_faceThreshold = 71;
    }
}

void UserIdRequest::run()
{
    int count = 0;
    while(true)
    {
        if(m_startFaceDownload)
        {
            if(m_updateFace.size() > 0)
            {
                if(m_updateFaceStatus)
                {
                    switchCtl->m_sync = true;
                    count = 0;
                    m_updateFaceStatus = false;
                    m_curFaceId = *m_updateFace.begin();
                    emit getUsers(m_curFaceId);
                }
            }
            else
            {
                emit sigInsertFail();
                sleep(1);
                updateIdentifyValue();
                switchCtl->m_sync = false;
                m_startFaceDownload = false;
            }
            if(!m_updateFaceStatus)
            {
                count++;
                if(count > 20 * switchCtl->m_tcpTimeout)
                {
                    if(m_updateFace.size() > 0)
                    {
                        m_updateFace.remove(m_curFaceId);
                        sqlDatabase->sqlInsertFailDelete(m_curFaceId);
                        sqlDatabase->sqlInsertFailInsert(m_curFaceId, "", "", 9);
                    }
                    count = 0;
                    m_updateFaceStatus = true;
                }
            }
            msleep(50);
        }
        else {
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
    m_updateFaceStatus = true;
    emit insertFaceGroups(id, name, edittime, photoName, mobile);
}

void UserIdRequest::tcpUpdateUsers(const QJsonObject &jsonObj)
{
    QString mobile = "", photoName = "";
    int id = jsonObj["mid"].toInt();
    m_updateFace.remove(id);
    QString name = jsonObj["username"].toString();
    QString photo = jsonObj["photo"].toString();
    QString edittime = jsonObj["updateDate"].toString();
    if(jsonObj.contains("mobile"))
    {
        mobile = jsonObj["mobile"].toString();
    }
    if(jsonObj.contains("photoName"))
    {
        photoName = jsonObj["photoName"].toString();
    }
    QFile file(QString::number(id) + ".jpg");
    file.open(QIODevice::ReadWrite);
    file.write(QByteArray::fromBase64(photo.toUtf8()));
    file.close();
    m_updateFaceStatus = true;
    emit insertFaceGroups(id, name, edittime, photoName, mobile);
}

void UserIdRequest::onUpdateUsers(const QJsonObject &jsonObj)
{
    if (jsonObj.isEmpty())
    {
        qDebug() << "user is nonexistent !";
        sqlDatabase->sqlInsertFailDelete(m_curFaceId);
        sqlDatabase->sqlInsertFailInsert(m_curFaceId, "", "", 9);
    }
    else
    {
        if(switchCtl->m_protocol)
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
    qDebug() << "OnAllUserId count:" << faceJson.count() << localFaceSet.size();
    m_updateFace.clear();
    if(faceJson.count() > 0)
    {
        foreach (QJsonValue val, faceJson)
        {
            QJsonObject jsonObj = val.toObject();
            int id = jsonObj["mid"].toInt();
            QString newTime = jsonObj["updateDate"].toString();
            QString updateTime = sqlDatabase->sqlSelectAllUserTime(id);
            if(updateTime.compare(newTime) != 0)
            {
                sqlDatabase->sqlDelete(id);
                m_updateFace.insert(id);
            }
            localFaceSet.remove(id);
        }
        foreach(int i, localFaceSet)
        {
            sqlDatabase->sqlDelete(i);
        }
        m_startFaceDownload = true;
    }
    qDebug() << m_updateFace.size();
    m_mutex.unlock();
}
