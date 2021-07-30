#include <QGuiApplication>
#include <QTranslator>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "prostorage.h"

int main(int argc, char *argv[])
{
    IF_VIO_VPSS_VO_MIPI_TX(switchCtl->m_camera, switchCtl->m_screen, switchCtl->m_angle);
    while (!IF_CheckYUVDataReady(0)) {
        QThread::msleep(1000);
    }
    QGuiApplication app(argc, argv);
//    QThread::currentThread()->setPriority(QThread::TimeCriticalPriority);
    QTranslator *m_translator = new QTranslator();
    switch (switchCtl->m_language) {
    case 0:
        m_translator->load("./language/OF_CH.qm");
        break;
    case 1:
        m_translator->load("./language/OF_EN.qm");
        break;
    case 2:
        m_translator->load("./language/OF_JA.qm");
        break;
    case 3:
        m_translator->load("./language/OF_KOR.qm");
        break;
    case 4:
        m_translator->load("./language/OF_RU.qm");
        break;
    case 5:
        m_translator->load("./language/OF_IN.qm");
        break;
    default:
        break;
    }
    app.installTranslator(m_translator);

    QQmlApplicationEngine engine;
    ProStorage *programs = new ProStorage;
    programs->start();
    engine.rootContext()->setContextProperty("programs", programs);
    engine.load(QUrl(QStringLiteral("./qml/main.qml")));
    return app.exec();
}
