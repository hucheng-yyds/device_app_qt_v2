#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QMutexLocker>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QLine>
#include <QDir>
#include <QTimer>
#include <QThread>
#include "datashare.h"

#define qt_debug() qDebug() << __FILE__ \
                            << __LINE__ \
                            << __FUNCTION__ \
                            << QThread::currentThreadId() \
                            << QTime::currentTime().addSecs(28800).toString("hh:mm:ss.zzz")



class Log : public QObject
{
    Q_OBJECT
public:
    explicit Log(QObject *parent = 0);

signals:
    void sigLogMsg(QString);
public slots:
    void onLogFun( bool );//调整日志输出的地方。true,日志输出到log文件，false，日志输出到配置工具
    void onToolTcpStateChange(bool state);//true:链接上了，false:链接断开
private:
   static void outputMessageOnLine(QtMsgType type, const QMessageLogContext &context, const QString &msg);
   static void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
   static QStringList logList;
   QTimer * timer;
};

#endif // LOG_H
