#include "log.h"
#include "datashare.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

QStringList Log::logList;

int m_fd = -1;
int m_fileSize = 0;

void Log::outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
//    QMutexLocker locker(&mutex);
    QDateTime dateTime = QDateTime::currentDateTime().addSecs(28800);
    QString path = dataShare->m_pathPrefix + "log/";
    QDir dir(path);
    if (!dir.exists()) {
        qt_debug() << path << dir.mkdir(path);
    }
    QStringList filesList = dir.entryList(QDir::Files|QDir::Readable, QDir::Time|QDir::Reversed);
    if (filesList.size() > 50) {
        for(int i = 0;i < 8;i++)
        {
            qt_debug() << QFile::remove(path + filesList[i]) << filesList.size();
        }
    }
    if(-1 == m_fd || m_fileSize > 2097152)
    {
        m_fileSize = 0;
        close(m_fd);
        usleep(10*1000);
        QString name = dateTime.toString("yyyy-MM-dd-HH-mm-ss") + "log.txt";
        QString filename = path + name;
        m_fd = open((char *)filename.toUtf8().data(), O_WRONLY | O_CREAT);
    }
    QString msgData = msg + "\n";
    if(m_fd > 0)
    {
        int wLen = write(m_fd, msgData.toUtf8().data(), msgData.size());
        m_fileSize += wLen;
    }
    sync();
}


void Log::outputMessageOnLine(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString tmp = (msg);
    Log::logList << tmp;
}

Log::Log(QObject *parent) : QObject(parent)
{
    timer = new QTimer(this);
    timer->setInterval(500);
    connect(timer,&QTimer::timeout,[=](){
        if( dataShare->m_log ==false)
        {
            foreach(auto item,Log::logList)
            {
                emit sigLogMsg(item);
                logList.removeOne(item);
            }
        }
    });
    qInstallMessageHandler(outputMessage);
}

void Log::onToolTcpStateChange(bool state)//true:链接上了，false:链接断开
{
    qt_debug() << state;
    if(!state)//
    {
        timer->stop();
        dataShare->m_log = true;
        close(m_fd);
        m_fd = -1;
        qInstallMessageHandler(outputMessage);
    }
}

//true 输出到文件
void Log::onLogFun(bool on)
{
    if(on)
    {
        dataShare->m_log = true;
        timer->stop();
        close(m_fd);
        m_fd = -1;
        qInstallMessageHandler(outputMessage);
    }
    else
    {
        dataShare->m_log = false;
        //立刻发送数据
        foreach(auto item,Log::logList)
        {
            emit sigLogMsg(item);
            logList.removeOne(item);
        }
        timer->start();
        qInstallMessageHandler(outputMessageOnLine);
        close(m_fd);
        m_fd = -1;
    }
}
