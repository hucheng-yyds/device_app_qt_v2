#include "icmodule.h"
#include "spicard.h"
#include "libHTRC.h"
#include "datashare.h"

IcCardModule::IcCardModule()
{

}

void IcCardModule::run()
{
    uint16_t status;
    status = OpenReader();

    if (status != ST_OK)
    {
        return ;
    }
    CardInfo card_check;
    status = QueryCard(&card_check);
    if(status == ST_OK)
    {
        sleep(10);
        while (1)
        {
            IDCardInfo IdInfo;
            CardInfo card;
            status = QueryCard(&card);
            if (status == ST_OK && card.CardType != CARD_TYPE_NONE)
            {
                if (card.CardType == CARD_TYPE_IDCARD)
                {
                    if(switchCtl->m_vi && icExpired())
                    {
                        emit readIcStatus(1);
                        //读身份证全信息
                        status = ReadIDCard("szofzn", "WYAxFsOeLTIeWlj4", nullptr, nullptr, 8885, &IdInfo);
                        system("rm sfz.bmp");
                        if (status == ST_OK)
                        {
                            FILE *fp = fopen("sfz.bmp", "w+");
                            if (fp != nullptr)
                            {
                                fwrite(IdInfo.BmpPhoto, 1, IdInfo.BitmapLen, fp);
                                fclose(fp);
                            }
                            icCountdown_ms(4*1000);
                            dataShare->m_idCardDatas.clear();
                            dataShare->m_idCardDatas << IdInfo.name << IdInfo.IdNumber << IdInfo.sex << IdInfo.Address << IdInfo.Birthday << IdInfo.nation;
                            dataShare->m_idCardFlag = true;
                            emit readIcStatus(2);
                        }
                        else
                        {
                            emit readIcStatus(0);
                            dataShare->m_idCardFlag = false;
                            hardware->playSound(tr("读卡失败").toUtf8(), "rkshibai.aac");
                        }
                    }
                }
                else
                {
                    dataShare->m_idCardFlag = false;
                    if(icExpired())
                    {
                        QByteArray byte;
                        for (uint8_t i = 0; i < card.CardNumbLen; i++)
                        {
                            byte.append(card.CardNumb[i]);
                        }
                        QString cardNo = QString::fromUtf8(byte.toHex()).toLower();
                        int time = 3*1000;
                        if(switchCtl->m_tempCtl)
                        {
                            time = 4*1000;
                        }
                        int mid = sqlDatabase->sqlSelectIcId(cardNo);
                        icCountdown_ms(time);
                        emit sigIcInfo(mid, cardNo);
                    }
                }
                msleep(200);
            }
            else
            {
                if(dataShare->m_idCardFlag)
                {
                    if(icExpired())
                    {
                        dataShare->m_idCardFlag = false;
                        hardware->playSound(tr("已超时").toUtf8(), "chaoshi.aac");
                    }
                }
                msleep(200);
            }
        }
    }
    else
    {
        CloseReader();
        qt_debug() << IF_InitSPICard();
        while (true)
        {
            _CardInfoType buf;
            IF_GetCardId(&buf);

            if(icExpired())
            {
                QByteArray byte;
                byte.append(buf.uCardId[0]);
                byte.append(buf.uCardId[1]);
                byte.append(buf.uCardId[2]);
                byte.append(buf.uCardId[3]);
                QString cardNo = QString::fromUtf8(byte.toHex()).toLower();
                int time = 3*1000;
                if(switchCtl->m_tempCtl)
                {
                    time = 4*1000;
                }
                int mid = sqlDatabase->sqlSelectIcId(cardNo);
                icCountdown_ms(time);
                emit sigIcInfo(mid, cardNo);
            }
            msleep(200);
        }
    }
}

bool IcCardModule::icExpired()
{
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time =QDateTime::currentDateTime();
    qint64 now = origin_time.msecsTo(current_date_time);
    if(now >= m_endTimerMs)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void IcCardModule::icCountdown_ms(int ms)
{
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time =QDateTime::currentDateTime();
    qint64 now = origin_time.msecsTo(current_date_time);
    m_endTimerMs = now + ms;
}
