#include "httpsclient.h"
#include "datashare.h"

HttpsClient::HttpsClient()
{
    moveToThread(this);
}

void HttpsClient::run()
{
    m_uploadStatus = false;
    m_httpAddr = switchCtl->m_httpAddr;
    m_tokenCmd = m_httpAddr + "/ouath/token";
    m_heartbeatCmd = m_httpAddr + "/device/heartbeat";
    m_getuserCmd = m_httpAddr + "/device/getuser/base64";
    m_uploadAccessCmd = m_httpAddr + "/device/uploadaccess/base64";
    m_alluserCmd = m_httpAddr + "/device/alluser";
    m_newupgrade = m_httpAddr + "/device/newupgrade/base64";
    m_unbindCmd = m_httpAddr + "/device/unbind";
    m_uploadFailFace = m_httpAddr + "/device/memberFaceFail";
    m_allowRequest = true;
    m_loginStatus = false;

    m_loginTimer = new QTimer;
    m_loginTimer->setInterval(5000);
    m_heartbeatTimer = new QTimer;
    m_heartbeatTimer->setInterval(30000);
    m_requestAllowTimer = new QTimer;
    m_requestAllowTimer->setInterval(10000);
    m_requestAllowTimer->setSingleShot(true);
    connect(m_loginTimer, &QTimer::timeout, this, &HttpsClient::HttpLogin);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &HttpsClient::HttpHeartbeat);
    connect(m_requestAllowTimer, &QTimer::timeout, this, [this]() {
        m_allowRequest = true;
    });
    m_loginTimer->start();
    exec();
}

int HttpsClient::httpsQRCode(const QString &data)
{
    int code = 1;
    QJsonObject jsonObj;
    jsonObj.insert("qrcodeStr", data);
    const QString &url = "http://120.79.147.36:8086/starr-web/device/qrcode/check";
    QJsonDocument document;
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader , "application/json");
    document.setObject(jsonObj);

    QTimer timer;
    timer.setInterval(15000);
    timer.setSingleShot(true);
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.post(request, document.toJson());
    QEventLoop eventloop;
    connect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &eventloop, &QEventLoop::quit);
    timer.start();
    eventloop.exec(QEventLoop::ExcludeUserInputEvents);

    if (timer.isActive())
    {
        timer.stop();
        if (reply->error() == QNetworkReply::NoError)
        {
            QJsonParseError jsonError;
            document = QJsonDocument::fromJson(reply->readAll(), &jsonError);
            if (jsonError.error != QJsonParseError::NoError)
            {
                qt_debug() << "jsonError.error" << jsonError.error;
            }
            else
            {
                jsonObj = document.object();
                if (jsonObj.contains("code"))
                {
                    code = jsonObj["code"].toInt();
                }
            }
            qt_debug() << "request post success" << url;
        }
        else
        {
            QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            qt_debug() << "status code: " << statusCodeV.toInt();
            qt_debug() << "status code: " << (int)reply->error();
        }
    }
    else
    {
        disconnect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
        reply->abort();
        qt_debug() << "post time out";
    }
    reply->deleteLater();

    return code;
}

bool HttpsClient::httpsUserReq(int id, const QString &url)
{
    qt_debug() << id << url;
    bool status = false;
    QNetworkRequest request;
    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1SslV3);
    request.setSslConfiguration(config);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader , "application/json");
    QTimer timer;
    timer.setInterval(5000);
    timer.setSingleShot(true);
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(request);
    QEventLoop eventloop;
    connect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &eventloop, &QEventLoop::quit);
    timer.start();
    eventloop.exec(QEventLoop::ExcludeUserInputEvents);

    if (timer.isActive())
    {
        timer.stop();
        if (reply->error() == QNetworkReply::NoError)
        {
            QString name = QString("%1.jpg").arg(id);
            QFile file(name);
            file.open(QIODevice::ReadWrite);
            file.write(reply->readAll());
            file.close();
            status = true;
            qt_debug() << "request image get success";
        }
        else
        {
            QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            qt_debug() << "get status code: " << statusCodeV.toInt();
            qt_debug() << "get status code: " << (int)reply->error();
        }
    }
    else
    {
        disconnect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
        reply->abort();
        qt_debug() << "get time out";
    }
    reply->deleteLater();
    qt_debug() << "end";
    return status;
}

void HttpsClient::httpsUnbind()
{
    requestPost(m_unbindCmd, QJsonObject());
}

void HttpsClient::httpsUploadopenlog(int id, int userId, const QString &photo, int isOver,int type, int isTemp, int sex, const QStringList &datas)
{
    if (!m_loginStatus || m_uploadStatus)
    {
        return;
    }
    m_uploadStatus = true;
    QJsonObject jsonObj;
    jsonObj.insert("userId", userId);
    jsonObj.insert("companyId", m_companyId);
    jsonObj.insert("sceneId", m_sceneId);
    jsonObj.insert("deviceId", 0);
    jsonObj.insert("unlockType", type);
    jsonObj.insert("photo", photo);
    jsonObj.insert("sn", switchCtl->m_sn);
    jsonObj.insert("unlockTime", datas.at(0));
    jsonObj.insert("temperature", datas.at(1));
    jsonObj.insert("isOver", isOver);
    jsonObj.insert("isTemp", isTemp);
    jsonObj.insert("isSuccess", datas.at(2).toInt());
    jsonObj.insert("invalidReason", datas.at(3));
    jsonObj.insert("isStranger", datas.at(4).toInt());
    jsonObj.insert("mjkh", datas.at(5));
    QJsonObject obj = requestPost(m_uploadAccessCmd, jsonObj);
    if(obj.contains("code"))
    {
        if(0 == obj["code"].toInt())
        {
            sqlDatabase->sqlDeleteOffline(id);
            qDebug() << "upload success" << id;
        }
    }
    else {
        qDebug() << "userId: " << id << type << userId << datas;
    }
    m_uploadStatus = false;
}

void HttpsClient::HttpsGetUsers(int id)
{
    QJsonObject jsonObj;
    QString oldPhotoName = sqlDatabase->sqlSelectPhotoName(id);
    jsonObj.insert("photoName", oldPhotoName);
    jsonObj.insert("mid", id);
    jsonObj = requestPost(m_getuserCmd, jsonObj);
    if (jsonObj.contains("code"))
    {
        if (0 == jsonObj["code"].toInt())
        {
            emit updateUsers(jsonObj["users"].toObject());
        }
    }
}

void HttpsClient::HttpsGetAllUserId()
{
    QJsonObject jsonObj = requestGet(m_alluserCmd);
    if (jsonObj.contains("code")) {
        qDebug() << "aluserId request" << jsonObj["code"].toInt();
        if (0 == jsonObj["code"].toInt())
        {
            emit allUserId(jsonObj["alluser"].toArray());
        }
    }
    else
    {
        qDebug() << "httpsGetAllUserID" << jsonObj;
    }
}

QJsonObject HttpsClient::requestGet(const QString &url)
{
    QJsonDocument document;
    qDebug() << url;
    if (m_allowRequest /*&& isOnline*/)
    {
        QNetworkRequest request;
        request.setUrl(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader , "application/json");
        request.setRawHeader("TOKEN", m_strKey.toLocal8Bit());

        QTimer timer;
        timer.setInterval(30000);
        timer.setSingleShot(true);
        QNetworkAccessManager manager;
        QNetworkReply *reply = manager.get(request);
        QEventLoop eventloop;
        connect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
        connect(&timer, &QTimer::timeout, &eventloop, &QEventLoop::quit);
        timer.start();
        eventloop.exec(QEventLoop::ExcludeUserInputEvents);

        if (timer.isActive())
        {
            timer.stop();
            if (reply->error() == QNetworkReply::NoError)
            {
                m_allowRequest = true;
                QJsonParseError jsonError;
                document = QJsonDocument::fromJson(reply->readAll(), &jsonError);
                if (jsonError.error != QJsonParseError::NoError)
                {
                    qDebug() << "jsonError.error" << jsonError.error;
                }
                qDebug() << "request get success";
            }
            else
            {
                QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
                qDebug() << "get status code: " << statusCodeV.toInt();
                qDebug() << "get status code: " << (int)reply->error();
            }
        }
        else
        {
            m_allowRequest = false;
            if(!m_loginTimer->isActive())
            {
                m_loginTimer->start();
                m_heartbeatTimer->stop();
            }
            m_requestAllowTimer->start();
            disconnect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
            reply->abort();
            dataShare->m_netStatus = false;
            qDebug() << "post time out";
        }
        reply->deleteLater();
    }
    return document.object();
}

QJsonObject HttpsClient::requestPost(const QString &url, const QJsonObject &jsonObj)
{
    qt_debug() << "request" << url;
    QJsonDocument document;
    QJsonObject obj;
    if (m_allowRequest/* && isOnline*/)
    {
        QNetworkRequest request;
        request.setUrl(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader , "application/json");
        request.setRawHeader("TOKEN", m_strKey.toLocal8Bit());
        document.setObject(jsonObj);

        QTimer timer;
        timer.setInterval(6000);
        timer.setSingleShot(true);
        QNetworkAccessManager manager;
        QNetworkReply *reply = manager.post(request, document.toJson());
        QEventLoop eventloop;
        connect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
        connect(&timer, &QTimer::timeout, &eventloop, &QEventLoop::quit);
        timer.start();
        eventloop.exec(QEventLoop::ExcludeUserInputEvents);

        if (timer.isActive())
        {
            timer.stop();
            if (reply->error() == QNetworkReply::NoError)
            {
                m_allowRequest = true;
                QJsonParseError jsonError;
                document = QJsonDocument::fromJson(reply->readAll(), &jsonError);
                if (jsonError.error == QJsonParseError::NoError)
                {
                    obj = document.object();
                    qt_debug() << "request post success";
                }              
            }
            else
            {
                QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
                qt_debug() << "status code: " << statusCodeV.toInt();
                qt_debug() << "status code: " << (int)reply->error();
            }
        }
        else
        {
            m_allowRequest = false;
            if(!m_loginTimer->isActive())
            {
                m_loginTimer->start();
                m_heartbeatTimer->stop();
            }
            m_requestAllowTimer->start();
            disconnect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
            reply->abort();
            dataShare->m_netStatus = false;
            qt_debug() << "post time out";
        }
        reply->deleteLater();
    }

    return obj;
}

QJsonObject HttpsClient::requestPostHeart(const QString &url, const QJsonObject &jsonObj)
{
    qt_debug() << "request" << url;
    QJsonDocument document;
    QJsonObject obj;
    if (m_allowRequest/* && isOnline*/)
    {
        QNetworkRequest request;
        request.setUrl(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader , "application/json");
        request.setRawHeader("TOKEN", m_strKey.toLocal8Bit());
        document.setObject(jsonObj);

        QTimer timer;
        timer.setInterval(5000);
        timer.setSingleShot(true);
        QNetworkAccessManager manager;
        QNetworkReply *reply = manager.post(request, document.toJson());
        QEventLoop eventloop;
        connect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
        connect(&timer, &QTimer::timeout, &eventloop, &QEventLoop::quit);
        timer.start();
        eventloop.exec(QEventLoop::ExcludeUserInputEvents);

        if (timer.isActive())
        {
            timer.stop();
            if (reply->error() == QNetworkReply::NoError)
            {
                m_allowRequest = true;
                QJsonParseError jsonError;
                document = QJsonDocument::fromJson(reply->readAll(), &jsonError);
                if (jsonError.error == QJsonParseError::NoError)
                {
                    obj = document.object();
                    qt_debug() << "request post success";
                }
            }
            else
            {
                QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
                qt_debug() << "status code: " << statusCodeV.toInt();
                qt_debug() << "status code: " << (int)reply->error();
            }
        }
        else
        {
            m_allowRequest = false;
            if(!m_loginTimer->isActive())
            {
                m_loginTimer->start();
                m_heartbeatTimer->stop();
            }
            m_requestAllowTimer->start();
            disconnect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
            reply->abort();
            dataShare->m_netStatus = false;
            qt_debug() << "post time out";
        }
        reply->deleteLater();
    }

    return obj;
}

void HttpsClient::HttpLogin()
{
    QJsonObject jsonObj;
    jsonObj.insert("sn", switchCtl->m_sn);
    jsonObj = requestPost(m_tokenCmd, jsonObj);
    qt_debug() << "httpsDoortoken" << jsonObj;
    if (jsonObj.contains("code"))
    {
        int code = jsonObj["code"].toInt();
        if (0 == code)
        {
            emit mqttReconnect();
            m_strKey = jsonObj["token"].toString();
            QJsonObject data = jsonObj["data"].toObject();
            m_companyId = data["companyId"].toString();
            m_sceneId = data["sceneId"].toString();
            m_loginStatus = true;
            switchCtl->m_devName = data["name"].toString();
            HttpsGetAllUserId();
            m_loginTimer->stop();
            m_heartbeatTimer->start();
            dataShare->m_netStatus = true;
            qt_debug() << "login success";
            return ;
        }
        qt_debug() << "login code:" << code;
    }
}

void HttpsClient::HttpHeartbeat()
{
    if(m_uploadStatus)
    {
        return;
    }
    m_uploadStatus = true;
    QJsonObject jsonObj;
    int count = sqlDatabase->m_localFaceSet.size();
    jsonObj.insert("peopleCount", count);
    jsonObj.insert("capacity", 30000);
    jsonObj.insert("wg", 0);
    jsonObj.insert("apkInfo", VERSION);
    jsonObj.insert("networkType", 1);
    jsonObj.insert("networkName", "??????");
    jsonObj.insert("ipAddr", dataShare->m_ipAddr);
    requestPostHeart(m_heartbeatCmd, jsonObj);
    m_uploadStatus = false;
}

int HttpsClient::httpsRequestImage(int id, const QString &url)
{
    int result = -1;
    QNetworkRequest request;
    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1SslV3);
    request.setSslConfiguration(config);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader , "application/json");

    QTimer timer;
    timer.setInterval(30000);
    timer.setSingleShot(true);
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(request);
    QEventLoop eventloop;
    connect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &eventloop, &QEventLoop::quit);
    timer.start();
    eventloop.exec(QEventLoop::ExcludeUserInputEvents);

    if (timer.isActive()) {
        timer.stop();
        if (reply->error() == QNetworkReply::NoError)
        {
            QString name = QString("%1.jpg").arg(id);
            QFile file(name);
            file.open(QIODevice::ReadWrite);
            file.write(reply->readAll());
            file.close();
            result = 0;
            qDebug() << "request image get success";
        }
        else {
            QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            qDebug() << "get status code: " << statusCodeV.toInt();
            qDebug() << "get status code: " << (int)reply->error();
        }
    } else {
        disconnect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
        reply->abort();
        qDebug() << "get time out";
    }
    reply->deleteLater();
    return result;
}

int HttpsClient::AlgorithmAuthorization()
{
    int code = 0;
    if (!QFile::exists("/mnt/usr/DV300_FaceArc_license/installed/"))
    {
        system("hasp_update_armesp_update_armhf_mv_himix200super f c2v");
        QFile file("c2v");
        file.open(QIODevice::ReadOnly);
        QString c2vStr = QString::fromUtf8(file.readAll().toBase64());
        file.close();
        file.remove();
        QJsonObject jsonObj;
        jsonObj.insert("sn", switchCtl->m_sn);
        jsonObj.insert("fp", c2vStr);
        QString auth = "http://120.79.147.36:9501/api/auth/";
        jsonObj = requestAuth(auth, jsonObj);
        qDebug() << jsonObj;
        if (jsonObj.contains("code")) {
            code = jsonObj["code"].toInt();
            if (0 == code) {
                c2vStr = jsonObj["data"].toString();
                file.setFileName("v2c");
                file.open(QIODevice::ReadWrite);
                file.write(QByteArray::fromBase64(c2vStr.toUtf8()));
                file.close();
                system("hasp_update_armesp_update_armhf_mv_himix200super u v2c");
                file.remove();
            } else if (10006 == code){
                system("rm -rf /mnt/usr/DV300_FaceArc_license/fs");
            }
        } else {
            system("rm -rf /mnt/usr/DV300_FaceArc_license/fs");
        }
        system("sync");
    }
    return code;
}

QJsonObject HttpsClient::requestAuth(const QString &url, const QJsonObject &jsonObj)
{
    qDebug() << "request" << url;
    QJsonDocument document;
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader , "application/json");
    document.setObject(jsonObj);

    QTimer timer;
    timer.setInterval(15000);
    timer.setSingleShot(true);
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.post(request, document.toJson());
    QEventLoop eventloop;
    connect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &eventloop, &QEventLoop::quit);
    timer.start();
    eventloop.exec(QEventLoop::ExcludeUserInputEvents);

    if (timer.isActive()) {
        timer.stop();
        if (reply->error() == QNetworkReply::NoError) {
            QJsonParseError jsonError;
            document = QJsonDocument::fromJson(reply->readAll(), &jsonError);
            if (jsonError.error != QJsonParseError::NoError) {
                qDebug() << "jsonError.error" << jsonError.error;
            }
            qDebug() << "request post success" << url;
        } else {
            QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
            qDebug() << "status code: " << statusCodeV.toInt();
            qDebug() << "status code: " << (int)reply->error();
        }
    } else {
        disconnect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
        reply->abort();
        qDebug() << "post time out";
    }
    reply->deleteLater();

    return document.object();
}

void HttpsClient::httpsDownload(const QString &url)
{
    QNetworkRequest request;
    QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1SslV3);
    request.setSslConfiguration(config);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader , "application/json");
    qt_debug() << "httpsDownload start" << url;
    QNetworkAccessManager manager;
    QNetworkReply *reply = manager.get(request);
    QEventLoop eventloop;
    connect(reply, &QNetworkReply::finished, &eventloop, &QEventLoop::quit);
//    connect(reply, &QNetworkReply::downloadProgress, this, &HttpsClient::downloadProgress);
    eventloop.exec(QEventLoop::ExcludeUserInputEvents);
    if (reply->error() == QNetworkReply::NoError) {
        QFile file("update.tar.xz");
        file.open(QIODevice::ReadWrite);
        qt_debug() << file.write(reply->readAll());
        file.close();
        qt_debug() << "httpsDownload success";
    } else {
        qt_debug() << reply->readAll();
        QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        qt_debug() << "get status code: " << statusCodeV.toInt();
        qt_debug() << "get status code: " << (int)reply->error();
    }

    qt_debug() << "httpsDownload end";
    reply->deleteLater();
}
