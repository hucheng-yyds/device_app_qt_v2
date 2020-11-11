#include "log.h"

static void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
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
        qt_debug() << QFile::remove(path + filesList[0]) << filesList.size();
    }
    QFile file(filename);
    if (file.size() > 1048576) {
        qt_debug() << file.rename(path + dateTime.toString("yyyy-MM-dd-HHmm") + "log.txt");
        file.setFileName(filename);
    }
//    qt_debug() << dateTime.addDays(-17).toString("dd");QCoreApplication::applicationDirPath()
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << msg << "\n";
    file.flush();
    file.close();
}

Log::Log(QObject *parent) : QObject(parent)
{

}

void Log::enable()
{
    qInstallMessageHandler(outputMessage);
}
