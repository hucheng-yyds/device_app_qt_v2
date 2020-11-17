#include "log.h"
#include "switch.h"
QStringList Log::logList;

void Log::outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
//    QMutexLocker locker(&mutex);
    QDateTime dateTime = QDateTime::currentDateTime().addSecs(28800);
    QString path = QDir::currentPath() + "/log/";
    QDir dir(path);
    if (!dir.exists()) {
        qt_debug() << path << dir.mkdir(path);
    }
    QString name = dateTime.toString("yyyy-MM-dd") + "log.txt";
    QString filename = path + name;
    QStringList filesList = dir.entryList(QDir::Files|QDir::Readable, QDir::Time|QDir::Reversed);
    if (filesList.size() > 50) {
        for(int i = 0;i < 8;i++)
        {
            qt_debug() << QFile::remove(path + filesList[i]) << filesList.size();
        }
    }
    QFile file(filename);
    if (file.size() > 1048576) {
        qt_debug() << file.rename(path + dateTime.toString("yyyy-MM-dd-HHmm") + "log.txt");
        file.setFileName(filename);
    }
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << msg << "\n";
    file.flush();
    file.close();
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

         if( switchCtl->m_log ==false)
         if(Log::logList.length()!=0)
         {
             for(int i = 0;i<Log::logList.length();i++)
             {
                emit sigLogMsg(Log::logList.at(i));
             }
             Log::logList.clear();
         }
     });
   // timer->start();
//    if(switchCtl->m_log)
//        qInstallMessageHandler(outputMessage);
//    else
//        qInstallMessageHandler(outputMessageOnLine);
}

void Log::onToolTcpStateChange(bool state)//true:链接上了，false:链接断开
{
    if(!state)//
    {
        qt_debug() << state;
        timer->stop();
        switchCtl->m_log = true;
        qInstallMessageHandler(outputMessage);
    }
}


void Log::onLogFun(bool on)
{
    if(on)
    {
        switchCtl->m_log = true;
        timer->stop();
        qInstallMessageHandler(outputMessage);

    }else {
        switchCtl->m_log = false;
        timer->start();
        qInstallMessageHandler(outputMessageOnLine);
    }
}
