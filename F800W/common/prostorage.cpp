#include "prostorage.h"
#include <QTimer>

ProStorage::ProStorage()
{
    moveToThread(this);
}

void ProStorage::init()
{
    FaceInterface *interFace = new FaceInterface;
    FaceManager *face = new FaceManager;
    connect(face, &FaceManager::showFaceFocuse, this, &ProStorage::showFaceFocuse);
    face->setFaceInter(interFace);
    FaceIdentify *identify = new FaceIdentify;
    identify->setFaceInter(interFace);
    connect(identify, &FaceIdentify::faceResultShow, this, &ProStorage::faceResultShow);
    bool status = face->init();
    while(!status)
    {
        msleep(500);
    }
    face->start();
    identify->start();
}

void ProStorage::run()
{
    exec();
}
