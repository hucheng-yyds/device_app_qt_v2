#include "prostorage.h"
#include <QImage>
#include <QProcess>
#include <QProcess>
#include <string>
#include <vector>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include "QrCode.hpp"
using namespace qrcodegen;

ProStorage::ProStorage()
{
}

void ProStorage::run()
{
    init();
    exec();
}

void ProStorage::init()
{
    DeviceSnJudgment();
    HttpsClient *httpClient = new HttpsClient;
    int code = httpClient->AlgorithmAuthorization();
    qt_debug() << "AlgorithmAuthorization:" << code;
    if(code != 0)
    {
        emit qrcodeChanged(QString("%1 %2").arg(tr("未授权")).arg(code));
        return;
    }
    Log *log = new Log;
//    log->enable();
    NetManager *netManager = new NetManager;
    connect(netManager, &NetManager::showDeviceInfo, this, &ProStorage::showDeviceInfo);
    connect(netManager, &NetManager::networkChanged, this, &ProStorage::networkChanged);
    connect(netManager, &NetManager::timeSync, this, &ProStorage::timeSync);
    netManager->start();
    FaceInterface *interFace = new FaceInterface;
    FaceManager *face = new FaceManager;
    connect(face, &FaceManager::showFaceFocuse, this, &ProStorage::showFaceFocuse);
    connect(face, &FaceManager::hideFaceFocuse, this, &ProStorage::hideFaceFocuse);
    connect(face, &FaceManager::faceTb, this, &ProStorage::faceTb);
    face->setFaceInter(interFace);
    FaceIdentify *identify = new FaceIdentify;
    identify->setFaceInter(interFace);
    connect(identify, &FaceIdentify::faceResultShow, this, &ProStorage::faceResultShow);
    TempManager *tempManager = new TempManager;
    connect(face, &FaceManager::endTemp, tempManager, &TempManager::endTemp);
    connect(identify, &FaceIdentify::startTemp, tempManager, &TempManager::startTemp);
    connect(identify, &FaceIdentify::showStartTemp, this, &ProStorage::showStartTemp);
    connect(identify, &FaceIdentify::tempShow, this, &ProStorage::tempShow);
    tempManager->start();

    IdCardModule *idcard = new IdCardModule;
    idcard->start();

    ToolTcpServer * toolTcpServer = new ToolTcpServer();
    toolTcpServer->start();

    MqttClient *mqttClient = new MqttClient;
    UserIdRequest *userRequest = new UserIdRequest;

    ServerDataList *serverList = new ServerDataList;
    mqttClient->setPacket(serverList);
    bool status = face->init();
    qt_debug() << "---------------init status:" << status;
    while(!status)
    {
        sleep(1);
    }
    float tempFlag = tempManager->onIsTemp();
    qt_debug() << "temp modle result:" << tempFlag;
    if(0 == tempFlag)
    {
        switchCtl->m_tempCtl = false;
        switchCtl->m_openMode = "Face";
        switchCtl->saveSwitchParam();
    }
    face->start();
    identify->start();
    userRequest->start();
    if(switchCtl->m_protocol)
    {
        ServerDataDeal *dataDeal = new ServerDataDeal;
        connect(dataDeal, &ServerDataDeal::insertFaceGroups, face, &FaceManager::insertFaceGroups);
        dataDeal->setHttp(httpClient);
        dataDeal->setPacket(serverList);
        TcpClient *tcpClient = new TcpClient;
        tcpClient->setPacket(serverList);
        connect(tcpClient, &TcpClient::allUserId, userRequest, &UserIdRequest::onAlluserId);
        connect(userRequest, &UserIdRequest::getUsers, tcpClient, &TcpClient::requestGetUsers);
        connect(tcpClient, &TcpClient::updateUsers, userRequest, &UserIdRequest::onUpdateUsers);
        connect(userRequest, &UserIdRequest::allUserId, tcpClient, &TcpClient::requestGetAllUserID);
        connect(tcpClient, &TcpClient::allUserIc, userRequest, &UserIdRequest::onAllUsersIc);
        connect(tcpClient, &TcpClient::newUserId, userRequest, &UserIdRequest::onNewUsers);
        connect(userRequest, &UserIdRequest::insertFaceGroups, face, &FaceManager::insertFaceGroups);
        connect(identify, &FaceIdentify::uploadopenlog, tcpClient, &TcpClient::uploadopenlog);
        connect(dataDeal, &ServerDataDeal::newUsers, userRequest, &UserIdRequest::onNewUsers);
        connect(userRequest, &UserIdRequest::sigInsertFail, tcpClient, &TcpClient::requestInserFail);
        connect(dataDeal, &ServerDataDeal::responseServer, tcpClient, &TcpClient::responseServer);
        connect(dataDeal, &ServerDataDeal::uploadopenlog, tcpClient, &TcpClient::uploadopenlog);
        tcpClient->start();
        dataDeal->start();
    }
    else {
        connect(identify, &FaceIdentify::uploadopenlog, httpClient, &HttpsClient::httpsUploadopenlog);
        connect(httpClient, &HttpsClient::allUserId, userRequest, &UserIdRequest::onAlluserId);
        connect(userRequest, &UserIdRequest::getUsers, httpClient, &HttpsClient::HttpsGetUsers);
        connect(httpClient, &HttpsClient::updateUsers, userRequest, &UserIdRequest::onUpdateUsers);
        connect(userRequest, &UserIdRequest::insertFaceGroups, face, &FaceManager::insertFaceGroups);
        httpClient->start();
    }
    mqttClient->start();
    emit syncSuccess(switchCtl->m_faceDoorCtl, switchCtl->m_tempCtl);
}

void ProStorage::DeviceSnJudgment()
{
    if(switchCtl->m_sn.isEmpty())
    {
        QString mac = "";
        QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
        foreach(QNetworkInterface i, list)
        {
            if(i.name().compare("eth0") == 0)
            {
                mac = "OFF1MJ" + i.hardwareAddress().replace(":", "");
                break;
            }
        }
        if(mac.isEmpty())
        {
            system("reboot");
        }
        else {
            switchCtl->m_sn = mac;
            system("echo " + mac.toUtf8() + " > /dev/mmcblk0p6");
            switchCtl->saveSreenParam();
        }
    }
    qt_debug() << "DeviceSnJudgment:" << switchCtl->m_sn;
    saveQRcodeImage(switchCtl->m_sn.toUtf8());
}

void ProStorage::saveQRcodeImage(const char *content)
{
    system("rm qrcode.jpg");
    std::vector<QrSegment> segs = QrSegment::makeSegments(content);
    QrCode qr1 = QrCode::encodeSegments(segs, QrCode::Ecc::HIGH, 5, 5, 2, false);
    //创建二维码画布
    QImage QrCode_Image = QImage(qr1.getSize(),qr1.getSize(),QImage::Format_RGB888);
    for (int y = 0; y < qr1.getSize(); y++) {
        for (int x = 0; x < qr1.getSize(); x++) {
            if(qr1.getModule(x, y)==0)
                QrCode_Image.setPixel(x,y,qRgb(255,255,250));
            else
                QrCode_Image.setPixel(x,y,qRgb(0,0,0));
        }
    }
    QrCode_Image=QrCode_Image.scaled(330, 330, Qt::KeepAspectRatio);
    QrCode_Image.save("qrcode.jpg");
}
