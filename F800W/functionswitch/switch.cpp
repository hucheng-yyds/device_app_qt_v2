#include "switch.h"

SwitchCtl::SwitchCtl()
{
    bool status = QFile::exists("./switch.json");
    if(!status)
    {
        setSwitchDefault();
    }
    status = QFile::exists("./screen.json");
    if(!status)
    {
        setScreenDefault();
    }
}

void SwitchCtl::saveSwitchParam(const QJsonObject &obj)
{
    QFile file("./switch.json");
    if(!file.open(QIODevice::ReadWrite)) {
        qDebug() << "File open failed!";
    } else {
        qDebug() <<"File open successfully!";
    }
    QJsonDocument jdoc(obj);
    file.seek(0);
    file.write(jdoc.toJson());
    file.flush();
    file.close();
}

QJsonObject SwitchCtl::readSwitchParam()
{
    QFile file("./switch.json");
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "File open failed!";
    } else {
        qDebug() <<"File open successfully!";
    }
    QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
    QJsonObject obj = jdc.object();
    file.close();
    return obj;
}

void SwitchCtl::setSwitchDefault()
{
    QFile file("./switch.json");
    if(!file.open(QIODevice::WriteOnly)) {
        qDebug() << "File open failed!";
    } else {
        qDebug() <<"File open successfully!";
    }

    QJsonObject obj;

    QJsonDocument jdoc(obj);
    file.write(jdoc.toJson());
    file.flush();
    file.close();
}

void SwitchCtl::saveSreenParam(const QJsonObject &obj)
{
    QFile file("./screen.json");
    if(!file.open(QIODevice::ReadWrite)) {
        qDebug() << "File open failed!";
    } else {
        qDebug() <<"File open successfully!";
    }
    QJsonDocument jdoc(obj);
    file.seek(0);
    file.write(jdoc.toJson());
    file.flush();
    file.close();
}

QJsonObject SwitchCtl::readScreenParam()
{
    QFile file("./screen.json");
    if(!file.open(QIODevice::ReadOnly)) {
        qDebug() << "File open failed!";
    } else {
        qDebug() <<"File open successfully!";
    }
    QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
    QJsonObject obj = jdc.object();
    file.close();
    return obj;
}

void SwitchCtl::setScreenDefault()
{
    QFile file("./switch.json");
    if(!file.open(QIODevice::WriteOnly)) {
        qDebug() << "File open failed!";
    } else {
        qDebug() <<"File open successfully!";
    }

    QJsonObject obj;
    obj.insert("angle", 1);
    obj.insert("camera", 22);
    obj.insert("screen", 4);
    QJsonDocument jdoc(obj);
    file.write(jdoc.toJson());
    file.flush();
    file.close();
}
