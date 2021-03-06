#include "offlinerecord.h"
#include "datashare.h"

OfflineRecord::OfflineRecord()
{

}

void OfflineRecord::run()
{
    QList<int> offlineRecords;
    while(true)
    {
        if(dataShare->m_netStatus && !dataShare->m_sync && dataShare->m_offlineFlag)
        {
            offlineRecords.clear();
            offlineRecords = sqlDatabase->sqlSelectAllOffLine();
            int count = offlineRecords.size();
            qt_debug() << "offlineRecords count:" << count;
            for(int i = count -1; i >=0; i--)
            {
                if(!dataShare->m_netStatus || dataShare->m_sync || !dataShare->m_offlineFlag)
                {
                    break;
                }
                QVariantList values = sqlDatabase->sqlSelectOffline(offlineRecords.at(i));
                if(values.isEmpty())
                {
                    continue;
                }
                QStringList texts;
                texts.clear();
                texts << values.at(3).toString() << values.at(4).toString() << values.at(7).toString()
                   << values.at(8).toString() << values.at(9).toString() << values.at(10).toString() << values.at(11).toString()
                      << values.at(13).toString() << values.at(14).toString() << values.at(15).toString() << values.at(16).toString();
                int ids = values.at(0).toInt();
                QString path = dataShare->m_offlinePath + QString::number(ids) + ".jpg";
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
                emit uploadopenlog(ids, values.at(1).toInt(), offlingImg, values.at(5).toInt(), values.at(2).toInt(), values.at(6).toInt(), values.at(12).toInt(), texts);
                sleep(5);
            }
            sleep(60);
        }
        else {
            sleep(1);
        }
    }
}
