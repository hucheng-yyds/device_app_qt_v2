#include "prostorage.h"
#include <QTimer>

ProStorage::ProStorage()
{
    init();
}

void ProStorage::init()
{
    NetManager *netManager = new NetManager;
    connect(netManager, &NetManager::showDeviceInfo, this, &ProStorage::showDeviceInfo);

    FaceInterface *interFace = new FaceInterface;
    FaceManager *face = new FaceManager;
    connect(face, &FaceManager::showFaceFocuse, this, &ProStorage::showFaceFocuse);
    connect(face, &FaceManager::hideFaceFocuse, this, &ProStorage::hideFaceFocuse);
    face->setFaceInter(interFace);
    FaceIdentify *identify = new FaceIdentify;
    identify->setFaceInter(interFace);
    connect(identify, &FaceIdentify::faceResultShow, this, &ProStorage::faceResultShow);
    bool status = face->init();
    qDebug() << "---------------init status:" << status;
    while(!status)
    {
        sleep(1);
    }
    face->start();
    identify->start();
    netManager->start();
}

