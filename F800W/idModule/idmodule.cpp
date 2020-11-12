#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include<unistd.h>
#include<sys/time.h>
#include <QTextCodec>
#include "idmodule.h"

IdCardModule::IdCardModule()
{
    countdown_ms(0);
}

void IdCardModule::run()
{
    struct timeval timeStart;
    struct timeval timeEnd;
    bool isSuccess = false;
    QString id, sex, name, nation, addr, birth;
    char strTemp[256] = {0x00};
    int strTempLen = sizeof(strTemp);
    QTextCodec *codec = QTextCodec::codecForName("utf-8");
    while(true)
    {
        msleep(300);
        int iRet = CVR_InitComm("/dev/ttyAMA3", 1);
        if(iRet != 1 && iRet != 2)
        {
            printf("CVR_InitComm failed, Ret = %d\n", iRet);
        }
        //读设备状态
        iRet = CVR_GetStatus();
        if(iRet != 1)
        {
            CVR_CloseComm();
            continue;
        }

        //卡认证
        iRet = CVR_Authenticate();
        if(iRet != 1)
        {
            if(switchCtl->m_idCardFlag)
            {
                if(expired())
                {
                    switchCtl->m_idCardFlag = false;
                    hardware->playSound(tr("已超时").toUtf8(), "chaoshi.aac");
                }
            }
            //找卡选卡
            unsigned int SNR = 0;
            unsigned short TagType = 0;
            iRet = CVR_MF_HL_Request(0, &SNR, &TagType);
            if(iRet != 1)
            {
//                printf("CVR_MF_HL_Request failed, Ret = %d\n", iRet);
                CVR_CloseComm();
                continue;
            }
            char dest[20];
            memset(dest, 0, 20);
            sprintf(dest, "%x", SNR);
            QString cardid = QString("%1%2%3%4%5%6%7%8").arg(dest[6]).arg(dest[7]).arg(dest[4]).arg(dest[5]).arg(dest[2]).arg(dest[3]).arg(dest[0]).arg(dest[1]);
//            QVariantList varList = sqlDatabase->sqlICSelect(cardid.toLower(), "cardNo");
//            emit idCardShowHome();
//            if (varList.isEmpty()) {
//                emit sigOpenDoor(false, 0, cardid.toLower());
//                if (settings->isTemp) {
//                    isSuccess = true;
//                } else {
//                    isSuccess = false;
//                }
//            } else {
//                int mid = varList.value(2).toInt();
//                emit sigOpenDoor(true, mid, varList.value(0).toString());
//                if (settings->isTemp) {
//                    isSuccess = true;
//                } else {
//                    isSuccess = false;
//                }
//            }
             qt_debug() << "ic read" << name << isSuccess;

            if (isSuccess) {
                sleep(6);
            } else {
                sleep(3);
            }
//            emit idCardFinish();
        }
        else if(expired() && switchCtl->m_vi)
        {
            //读卡
            iRet = CVR_Read_Content(2);
            if(iRet != 1)
            {
                qt_debug() << "CVR_Read_FPContent failed, Ret = " << iRet;
                continue;
            }

            //获取卡类型
            strTempLen = sizeof(strTemp);
            memset(strTemp, 0, strTempLen);
            iRet = GetCertType((unsigned char*)strTemp, &strTempLen);
            if(iRet != 1)
            {
                qt_debug() << "GetCertType failed, Ret = " << iRet;
                continue;
            }
            //获取姓名
            strTempLen = sizeof(strTemp);
            memset(strTemp, 0, strTempLen);
            iRet = GetPeopleName(strTemp, &strTempLen);
            if(iRet != 1)
            {
                qt_debug() << "GetPeopleName failed, Ret =" << iRet;
                continue;
            }
            name = QString(codec->toUnicode(QByteArray(strTemp)));
            //获取性别
            strTempLen = sizeof(strTemp);
            memset(strTemp, 0, strTempLen);
            iRet = GetPeopleSex(strTemp, &strTempLen);
            if(iRet != 1)
            {
                qt_debug() << "GetPeopleSex failed, Ret =" << iRet;
                continue;
            }
            sex = QString(codec->toUnicode(QByteArray(strTemp)));

            //获取民族/国家地区
            strTempLen = sizeof(strTemp);
            memset(strTemp, 0, strTempLen);
            iRet = GetPeopleNation(strTemp, &strTempLen);
            if(iRet != 1)
            {
                qt_debug() << "GetPeopleNation failed, Ret =" << iRet;
                continue;
            }
            nation = QString(codec->toUnicode(QByteArray(strTemp)));;

            strTempLen = sizeof(strTemp);
            memset(strTemp, 0, strTempLen);
            iRet = GetPeopleIDCode(strTemp, &strTempLen);
            if(iRet != 1)
            {
                qt_debug() << "GetPeopleIDCode failed, Ret =" << iRet;
                continue;
            }
            id = QString(QLatin1String(strTemp));

            strTempLen = sizeof(strTemp);
            memset(strTemp, 0, strTempLen);
            iRet = GetPeopleBirthday(strTemp, &strTempLen);
            if(iRet != 1)
            {
                qt_debug() << "GetPeopleBirthday failed, Ret =" << iRet;
                continue;
            }
            birth = QString(QLatin1String(strTemp));

            strTempLen = sizeof(strTemp);
            memset(strTemp, 0, strTempLen);
            iRet = GetPeopleAddress(strTemp, &strTempLen);
            if(iRet != 1)
            {
                qt_debug() << "GetPeopleAddress failed, Ret =" << iRet;
                continue;
            }
            addr = QString(codec->toUnicode(QByteArray(strTemp)));;

            //获取头像照片
            unsigned char arrPhotoData[38862];
            int iPhotoDataLen = sizeof(arrPhotoData);
            iRet = GetBMPData(arrPhotoData, &iPhotoDataLen);
            if(iRet != 1)
            {
                qt_debug() << "GetBMPData failed, Ret =" << iRet;
                continue;
            }

            FILE* fp_photo = fopen("./sfz.bmp","wb+");
            if(fp_photo == nullptr)
            {
                qt_debug() << "write  Photo error";
            }
            fwrite(arrPhotoData, 1, iPhotoDataLen, fp_photo);
            fclose(fp_photo);
            countdown_ms(4*1000);
            switchCtl->m_idCardDatas << name << id << sex << addr << birth << nation;
            switchCtl->m_idCardFlag = true;
        }
        else {
        }
        CVR_CloseComm();
    }
}

bool IdCardModule::expired()
{
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time =QDateTime::currentDateTime();
    qint64 now = origin_time.msecsTo(current_date_time);
    if(now >= m_endTimerMs)
    {
        return true;
    }
    else {
        return false;
    }
}

void IdCardModule::countdown_ms(int ms)
{
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time =QDateTime::currentDateTime();
    qint64 now = origin_time.msecsTo(current_date_time);
    m_endTimerMs = now + ms;
}
