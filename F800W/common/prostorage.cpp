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
    init();
}

void ProStorage::init()
{
    DeviceSnJudgment();
    HttpsClient *httpClient = new HttpsClient;
    int code = httpClient->AlgorithmAuthorization();
    qDebug() << "AlgorithmAuthorization:" << code;
    if(code != 0)
    {
        emit qrcodeChanged(QString("%1 %2").arg(tr("未授权")).arg(code));
        return;
    }

    NetManager *netManager = new NetManager;
    connect(netManager, &NetManager::showDeviceInfo, this, &ProStorage::showDeviceInfo);

    FaceInterface *interFace = new FaceInterface;
    FaceManager *face = new FaceManager;
    connect(face, &FaceManager::showFaceFocuse, this, &ProStorage::showFaceFocuse);
    connect(face, &FaceManager::hideFaceFocuse, this, &ProStorage::hideFaceFocuse);
    connect(face, &FaceManager::syncSuccess, this, &ProStorage::syncSuccess);
    face->setFaceInter(interFace);
    FaceIdentify *identify = new FaceIdentify;
    identify->setFaceInter(interFace);
    connect(identify, &FaceIdentify::faceResultShow, this, &ProStorage::faceResultShow);
    TempManager *tempManager = new TempManager;
    connect(face, &FaceManager::endTemp, tempManager, &TempManager::endTemp);
    connect(identify, &FaceIdentify::startTemp, tempManager, &TempManager::startTemp);
    connect(identify, &FaceIdentify::showStartTemp, this, &ProStorage::showStartTemp);
    connect(identify, &FaceIdentify::tempShow, this, &ProStorage::tempShow);
    connect(tempManager, &TempManager::sendTempResult, identify, &FaceIdentify::recvTempResult);
    tempManager->start();
    bool status = face->init();
    qDebug() << "---------------init status:" << status;
    while(!status)
    {
        sleep(1);
    }
    float tempFlag = tempManager->onIsTemp();
    qDebug() << "temp modle result:" << tempFlag;
    if(0.0 == tempFlag)
    {
        switchCtl->m_tempCtl = false;
        switchCtl->m_openMode = "Face";
        switchCtl->saveSwitchParam();
    }
    face->start();
    identify->start();
    netManager->start();
}

void ProStorage::DeviceSnJudgment()
{
    if(switchCtl->m_sn.isEmpty())
    {
        QString outputStr;
        QByteArray mac;
        QProcess* process = new QProcess;
        process->start("cat /sys/kernel/swsensor/id");
        process->waitForFinished();
        outputStr = QString::fromLocal8Bit(process->readAllStandardOutput());
        qDebug() << outputStr;
        mac = ("OFF1MJ" + outputStr.mid(0,12)).toLocal8Bit();
        switchCtl->m_sn = mac;
        system("echo " + mac + " > /dev/mmcblk0p6");
        switchCtl->saveSreenParam();
    }
    qDebug() << "DeviceSnJudgment:" << switchCtl->m_sn;
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
