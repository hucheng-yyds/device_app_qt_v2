#include "offlinerecord.h"

OfflineRecord::OfflineRecord()
{

}

void OfflineRecord::run()
{
    QList<int> offlineRecords;
    while(true)
    {
        if(switchCtl->m_netStatus && !switchCtl->m_sync && switchCtl->m_offlineFlag)
        {
            offlineRecords.clear();
            offlineRecords = sqlDatabase->sqlSelectAllOffLine();
            int count = offlineRecords.size();
            for(int i = count -1; i >=0; i--)
            {
                if(!switchCtl->m_netStatus || switchCtl->m_sync)
                {
                    break;
                }
                QVariantList values = sqlDatabase->sqlSelectOffline(i);
                if(values.isEmpty())
                {
                    continue;
                }
                QStringList texts;
                texts.clear();
                texts << values.at(3).toString() << values.at(4).toString() << values.at(7).toString()
                   << values.at(8).toString() << values.at(9).toString() << values.at(10).toString();
                int ids = values.at(0).toInt();
                QString path = "offline/" + QString::number(ids) + ".jpg";
                QFile file(path);
                QString offlingImg = "";
                if(file.open(QIODevice::ReadWrite))
                {
                    offlingImg = QString::fromUtf8(file.readAll().toBase64());
                }
                else
                {
                    qt_debug() << path << "open fail";
                }
                emit uploadopenlog(ids, values.at(1).toInt(), offlingImg, values.at(5).toInt(), values.at(2).toInt(), values.at(6).toInt(), texts);
                sleep(2);
            }
            sleep(60);
        }
        else {
            sleep(1);
        }
    }
}
