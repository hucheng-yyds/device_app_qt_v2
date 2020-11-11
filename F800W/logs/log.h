#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QMutexLocker>
#include <QDebug>
#include <QDateTime>
#include <QFile>
#include <QLine>
#include <QDir>
#include <QThread>
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
    void enable();

signals:

public slots:
};

#endif // LOG_H
