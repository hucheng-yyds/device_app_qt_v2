#include "faceidentify.h"
#include "datashare.h"

extern QSemaphore g_usedSpace;
extern bool g_idRead;
FaceIdentify::FaceIdentify()
{
    m_interFace = nullptr;
    m_tempFlag = false;
    m_cardWork = false;
    m_tempVal = "0";
    m_tempResult = 0;
    m_irImage = new unsigned char[VIDEO_WIDTH*VIDEO_HEIGHT* 3 / 2];
    m_bgrImage = new unsigned char[VIDEO_WIDTH*VIDEO_HEIGHT* 3 / 2];
}

//bool FaceIdentify::idCardFaceComparison(char *feature_result)
//{
//    int size = 0;
//    float result = 0;
//    QString file = "sfz.bmp";
//    char *featureFrist;
//    FaceHandle *faceHandle = nullptr;
//    int count = 0;
//    bool idPass = false;
//    auto image = cv::imread(file.toStdString());
//    if(!image.empty())
//    {
//        FacePoseBlur pose_blur;
//        detect((const char *)image.data, image.cols, image.rows, BGR, 0.75, &faceHandle, &count);
//        if(count > 0)
//        {
//            extract(faceHandle[0], &featureFrist, &size);
//            compare(featureFrist, feature_result, size, &result);
//            QString name = dataShare->m_idCardDatas.at(0);
//            if (result > switchCtl->m_idcardValue)
//            {
//                idPass = true;
//                emit idCardResultShow(2, name, tr("认证通过"), m_faceInfo + name);
//                hardware->playSound(tr("认证通过").toUtf8(), "chengong.aac");
//            }
//            else
//            {
//                idPass = false;
//                emit idCardResultShow(1, name, tr("认证失败"), name);
//                hardware->playSound(tr("认证失败").toUtf8(), "rzshibai.aac");
//            }
//            releaseFeature(featureFrist);
//            releaseAllFace(faceHandle, count);
//        }
//    }
//    return idPass;
//}

void FaceIdentify::judgeDate()
{
    if(switchCtl->m_language)
    {
        m_faceInfo = "";
        return;
    }
    QString curTime = QDateTime::currentDateTime().addSecs(28800).toString("HH:mm:ss");
    if(curTime.compare("01:00:00") > 0 && curTime.compare("12:00:00") < 0)
    {
        m_faceInfo = tr("早上好，");
    }
    else if(curTime.compare("12:00:00") > 0 && curTime.compare("18:30:00") < 0)
    {
        m_faceInfo = tr("下午好，");
    }
    else
    {
        m_faceInfo = tr("晚上好，");
    }
}

void FaceIdentify::run()
{
//    int j = 0, m_count = 0;
//    while (1) {
//        Countdown timer(300);
//        if (j >= 100) {
//            double d_ms = m_count / 100.00;
//            qt_debug() << "FaceIdentify m_count:" << m_count << d_ms;
//            j = 0;
//            m_count = 0;
//        }
//        j ++;
//        m_count += timer.right_ms();
//        msleep(20);
//    }
    while(1) {
        g_usedSpace.acquire();
        int i = 0;
        QVector<DS_FaceInfo> &m_iMFaceHandle = m_interFace->m_faceHandle;
        int trId = m_iMFaceHandle[i].trackID/*[0] + m_iMFaceHandle[i].trackID[1]*/;
        bool ir = switchCtl->m_ir;
        bool faceDoor = switchCtl->m_faceDoorCtl;
        bool tempCtl = switchCtl->m_tempCtl;
        bool vi = switchCtl->m_vi;
        QString openMode = switchCtl->m_openMode;

        QStringList datas;
        bool authority = false;
        bool egPass = false;
        bool tempPass = false;
        QString isSuccess = "0";
        QString isStranger = "0";
        QString invalidReason = "";
        uint64_t face_id = (uint64_t)m_iMFaceHandle[i].ID;
        QString uploadTime = "";
        int offlineNmae = 0;
        QString cardNo = "";
        int isOver = 0;
        QString snapshot = "";

        qt_debug() << "ptrFaceInfo.pose:" << m_iMFaceHandle[i].pose;
        qt_debug() << "ptrFaceInfo.recStatus:" << m_iMFaceHandle[i].recStatus;
        qt_debug() << "ptrFaceInfo.recScoreVal:" << m_iMFaceHandle[i].recScoreVal;
        qt_debug() << "ptrFaceInfo.faceMaskOrNot:" << m_iMFaceHandle[i].faceMaskOrNot;

        if (RETURN_REC_SUCCESS == m_iMFaceHandle[i].recStatus) {
//            if (identify->track_id.value(i) != trId) {
//                timer.countdown(3);
//            } else {
//                if (timer.expired()) {
//                    timer.countdown(3);
//                } else {
//                    qt_debug() << "22222222222sid:" << trId;
//                    goto exit;
//                }
//            }
//            QByteArray faceByte = QByteArray((const char*)m_iMFaceHandle[i].ID, 16);
//            int id = m_iMFaceHandle[i].ID;
            qt_debug() << "m_iMFaceHandle[i].ID:" << m_iMFaceHandle[i].ID;
//            QVariantList varlist = sqlDatabase->sqlSelect((uint64_t)m_iMFaceHandle[i].ID);
//            QString name = varlist.value(1).toString();
//            qt_debug() << name;
            judgeDate();
            QStringList value = dealOpencondition(face_id);
            QString name = value.at(0);
            QString remark = value.at(1);
            if(name.isEmpty())
            {
                authority = true;
                emit faceResultShow(name, i, trId, tr("未授权"), tr("未授权"));
                if(remark.isEmpty()) {
//                            hardware->playSound(tr("未授权").toUtf8(), "authority.aac");
                    hardware->playSound("authority.aac");
                } else {
//                            hardware->playSound(remark.toUtf8(), "authority.aac");
                    hardware->playSound("authority.aac");
                }
            }
            else
            {
                if(0 == switchCtl->m_language)
                {
                    if(1 == switchCtl->m_nameMask)
                    {
                        if(name.size() > 3)
                        {
                            name = name.replace(0, 2, "**");
                        }
                        else if(name.size() > 1){
                            name = name.replace(0, 1, '*');
                        }
                    }
                    else if(2 == switchCtl->m_nameMask)
                    {
                        name = name.replace(0, name.size(), tr("您好"));
                    }
                }
                emit faceResultShow(name, i, trId, tr("认证通过"), m_faceInfo + name);
                egPass = true;
                hardware->playSound("chengong.wav");
            }
            if(tempCtl) {
                m_tempFlag = false;
                emit startTemp();
                emit showStartTemp();
                qt_debug() << "============" << dataShare->m_tempFlag;
                while (!dataShare->m_tempFlag) {
                    msleep(100);
                }
                m_tempVal = dataShare->m_tempVal;
                m_tempResult = dataShare->m_tempResult;
                if(switchCtl->m_fahrenheit)
                {
                    emit tempShow(QString("%1℉").arg(m_tempVal), m_tempResult);
                }
                else
                {
                    emit tempShow(QString("%1℃").arg(m_tempVal), m_tempResult);
                }
                dataShare->m_tempFlag = false;
//                emit tempShow(m_tempVal, m_tempResult);
                msleep(150);
                qt_debug() << m_tempVal << m_tempResult;
                float warnValue = switchCtl->m_warnValue;
                if(switchCtl->m_fahrenheit)
                {
                    warnValue = (warnValue)*1.8 + 32.0;
                }
                if (m_tempVal.toFloat() >= warnValue)
                {
                    tempPass = false;
                }
                else if(m_tempResult > 0)
                {
                    tempPass = true;
                }
                if(-1 == m_tempResult)
                {
                    tempPass = false;
                    hardware->playSound("tiwenlow.wav");
                }
                else if(-2 == m_tempResult)
                {
                    tempPass = false;
                    hardware->playSound("cwshibai.wav");
                }
                else {
                    if (tempPass) {
                        hardware->playSound("tiwenzhc.wav");
                    } else {
                        hardware->playSound("tiwenyc.wav");
                    }
                }
            } else {
//                QStringList value = dealOpencondition(face_id);
//                QString name = value.at(0);
//                QString remark = value.at(1);
//                if(name.isEmpty())
//                {
//                    authority = true;
//                    emit faceResultShow(name, i, trId, tr("未授权"), tr("未授权"));
//                    if(remark.isEmpty()) {
////                            hardware->playSound(tr("未授权").toUtf8(), "authority.aac");
//                        hardware->playSound("authority.aac");
//                    } else {
////                            hardware->playSound(remark.toUtf8(), "authority.aac");
//                        hardware->playSound("authority.aac");
//                    }
//                }
//                else
//                {
//                    if(0 == switchCtl->m_language)
//                    {
//                        if(1 == switchCtl->m_nameMask)
//                        {
//                            if(name.size() > 3)
//                            {
//                                name = name.replace(0, 2, "**");
//                            }
//                            else if(name.size() > 1){
//                                name = name.replace(0, 1, '*');
//                            }
//                        }
//                        else if(2 == switchCtl->m_nameMask)
//                        {
//                            name = name.replace(0, name.size(), tr("您好"));
//                        }
//                    }
//                    emit faceResultShow(name, i, trId, tr("认证通过"), m_faceInfo + name);
//                    egPass = true;
//                    hardware->playSound("chengong.wav");
//                }
            }
        } else if (RETURN_RECOGING == m_iMFaceHandle[i].recStatus) {
            if (/*m_iMFaceHandle[i].recScoreVal > 0 &&
                    */m_iMFaceHandle[i].recScoreVal < 0.75 &&
                    m_iMFaceHandle[i].pose == 0 /*&&
                    identify->track_id.value(i) == trId &&
                    identify->c_timer->expired()*/) {
//                if (timer.expired()) {
//                    timer.countdown(3);
//                } else {
//                    qt_debug() << "22222222222sid:" << trId;
//                    goto exit;
//                }
//                emit faceResultShow(tr("未注册"), i, trId, tr("未注册"), tr("未注册"));
                isStranger = "1";
                emit faceResultShow(tr("未注册"), i, trId, tr("请联系管理员"), tr("未注册"));
                egPass = false;
                face_id = 0;
                hardware->playSound("shibai.wav");
            } else if (m_iMFaceHandle[i].pose > 0) {
//                emit pose();
                goto exit;
            } else {
                goto exit;
            }
        } else {
            goto exit;
        }
        if (egPass) {
            hardware->checkOpenDoor();
        }
        if (switchCtl->m_uploadImageCtl)
        {
            cv::Mat nv21(VIDEO_HEIGHT + VIDEO_HEIGHT / 2, VIDEO_WIDTH, CV_8UC1, m_interFace->m_bgrImage);
            cv::Mat image;
            cv::cvtColor(nv21, image, CV_YUV2BGR_NV12);
            if(image.empty())
            {
                printf("load image error!!\n");
                goto exit;
            }
            QVector<int> opts;
            opts.push_back(cv::IMWRITE_JPEG_QUALITY);
            opts.push_back(30);
            opts.push_back(cv::IMWRITE_JPEG_OPTIMIZE);
            opts.push_back(1);
            cv::imwrite("snap.jpg", image, opts.toStdVector());
            QFile file("snap.jpg");
            file.open(QIODevice::ReadWrite);
            snapshot = QString::fromUtf8(file.readAll().toBase64());
            file.close();
            offlineNmae = QDateTime::currentDateTime().toTime_t();
            QString offline_path = "cp snap.jpg offline/" + QString::number(offlineNmae) + ".jpg";
            system(offline_path.toStdString().c_str());
        }
        isOver = tempPass ? 1 : 0;
        datas.clear();
        uploadTime = QDateTime::currentDateTime().addSecs(28800).toString("yyyy-MM-dd HH:mm:ss");
        datas << uploadTime << m_tempVal << isSuccess << invalidReason << isStranger << m_cardNo;
        if((switchCtl->m_uploadStrangerCtl || egPass) && !vi)
        {
            if(m_cardWork && !tempCtl)
            {
            }
            else {
                if(dataShare->m_netStatus)
                {
                    emit uploadopenlog(offlineNmae, face_id, snapshot, isOver, 1, tempCtl, datas);
                }
                sqlDatabase->sqlInsertOffline(offlineNmae, face_id, 1, isOver, tempCtl, datas);
            }
        }
exit:
        m_interFace->m_faceHandle.clear();
//        identify->track_id.clear();
//        identify->track_id << trId;
//        qt_debug() << "identify->track_id:" << identify->track_id.first();
        m_interFace->m_iStop = true;
    }
}

void FaceIdentify::dealIcData(int mid, const QString &cardNo)
{
//    if(cardNo.isEmpty())
//    {
//        m_cardWork = false;
//    }
//    else {
//        m_cardWork = true;
//    }
//    m_cardNo = cardNo;
//    bool tempCtl = switchCtl->m_tempCtl;
//    if(tempCtl)
//    {
//        identifyCountdown_ms(3*1000);
//    }
//    QString isSuccess = "0";
//    QString isStranger = "0";
//    if(mid > 0)
//    {
//        QStringList value = dealOpencondition(mid);
//        QString name = value.at(0);
//        QString remark = value.at(1);
//        if(name.isEmpty())
//        {
//            emit icResultShow(1, tr("未授权"), tr("未授权"));
//            if(remark.isEmpty())
//            {
//                hardware->playSound(tr("未授权").toUtf8(), "authority.aac");
//            }
//            else {
//                hardware->playSound(remark.toUtf8(), "authority.aac");
//            }
//            isSuccess = "0";
//        }
//        else
//        {
//            if(0 == switchCtl->m_language)
//            {
//                if(1 == switchCtl->m_nameMask)
//                {
//                    if(name.size() > 3)
//                    {
//                        name = name.replace(0, 2, "**");
//                    }
//                    else if(name.size() > 1)
//                    {
//                        name = name.replace(0, 1, '*');
//                    }
//                }
//                else if(2 == switchCtl->m_nameMask)
//                {
//                    name = name.replace(0, name.size(), tr("您好"));
//                }
//            }
//            emit icResultShow(1, name, m_faceInfo + name);
//            if(tempCtl)
//            {
//                hardware->playSound(tr("请看摄像头").toUtf8(), "kansxt.aac");
//            }
//            else {
//                hardware->playSound(tr("认证通过").toUtf8(), "chengong.aac");
//                emit wgOut(cardNo.toUtf8());
//                hardware->ctlLed(GREEN);
//                hardware->checkOpenDoor();
//                isSuccess = "1";
//            }
//        }
//        isStranger = "0";
//    }
//    else {
//        emit icResultShow(0, tr("未注册"), tr("未注册"));
//        if(tempCtl)
//        {
//            hardware->playSound(tr("请看摄像头").toUtf8(), "kansxt.aac");
//        }
//        else {
//            hardware->playSound(tr("未注册").toUtf8(), "shibai.aac");
//            hardware->ctlLed(RED);
//            isSuccess = "0";
//            isStranger = "1";
//        }
//    }
//    if(!tempCtl)
//    {
//        QStringList datas;
//        datas.clear();
//        int offlineNmae = QDateTime::currentDateTime().toTime_t();
//        datas << QDateTime::currentDateTime().addSecs(28800).toString("yyyy-MM-dd HH:mm:ss") << "" << isSuccess << "" << isStranger << cardNo;
//        emit uploadopenlog(offlineNmae, mid, "", 0, 3, 0, datas);
//        sqlDatabase->sqlInsertOffline(offlineNmae, mid, 3, 0, 0, datas);
//        m_cardWork = false;
//        m_cardNo = "";
//    }
}

QStringList FaceIdentify::dealOpencondition(int faceId)
{
    bool pass = false;
    QString name = "";
    int status = 0;
    QStringList text;
    text.clear();
    QVariantList value = sqlDatabase->sqlSelectAuth(faceId);
    if(value.size() <= 0)
    {
        name = sqlDatabase->sqlSelect(faceId).value(1).toString();
        text << name << "";
        return text;
    }
    qt_debug() << value;
    int passnum = value[1].toInt();
    QString startTime = value[2].toString();
    QString expireTime = value[3].toString();
    int isBlack = value[4].toInt();
    QString passPeriod = value[5].toString();
    QString passTimeSection = value[6].toString();
    QString remark = "";
    if(1 == isBlack)
    {
        remark = value[7].toString();
        text << name << remark;
        return text;
    }
    QString curTime = QDateTime::currentDateTime().addSecs(28800).toString("yyyy-MM-dd HH:mm:ss");
    if(startTime.isEmpty() && expireTime.isEmpty() && passnum != 0)
    {
        pass = true;
    }
    else if(startTime.compare("always") == 0 && expireTime.compare("always") == 0 && passnum != 0)
    {
        pass = true;
    }
    else if(curTime.compare(startTime) >= 0 && curTime.compare(expireTime) <= 0 && passnum != 0)
    {
        pass = true;
    }
    else {
        pass = false;
    }
    if(pass)
    {
        pass = false;
        if(passPeriod.isEmpty() || passPeriod.compare("always") == 0)
        {
            pass = true;
        }
        else {
            QDateTime current_date_time = QDateTime::currentDateTime();
            QString current_week = current_date_time.toString("ddd");
            if(0 == current_week.compare("Mon"))
            {
                status = passPeriod.mid(0, 1).toInt();
            }
            else if(0 == current_week.compare("Tue"))
            {
                status = passPeriod.mid(1, 1).toInt();
            }
            else if(0 == current_week.compare("Wed"))
            {
                status = passPeriod.mid(2, 1).toInt();
            }
            else if(0 == current_week.compare("Thu"))
            {
                status = passPeriod.mid(3, 1).toInt();
            }
            else if(0 == current_week.compare("Fri"))
            {
                status = passPeriod.mid(4, 1).toInt();
            }
            else if(0 == current_week.compare("Sat"))
            {
                status = passPeriod.mid(5, 1).toInt();
            }
            else if(0 == current_week.compare("Sun"))
            {
                status = passPeriod.mid(6, 1).toInt();
            }
            if(1 == status)
            {
                pass = true;
            }
        }
    }
    if(pass)
    {
        pass = false;
        if(passTimeSection.isEmpty() || passTimeSection.compare("always") == 0)
        {
            pass = true;
        }
        else
        {
            QStringList list = passTimeSection.split(",");
            QString timeStr = QTime::currentTime().addSecs(28800).toString("hh:mm");
            foreach (QString time, list) {
                QStringList timeList = time.split("~");
                if (timeStr > timeList.value(0) && timeStr < timeList.value(1)) {
                    pass = true;
                }
            }
        }
    }
    if(pass)
    {
        name = sqlDatabase->sqlSelect(faceId).value(1).toString();
        if(passnum >= 1)
        {
            sqlDatabase->sqlUpdatePassNum(faceId, passnum-1);
        }
    }
    remark = value[7].toString();
    text << name << remark;
    return text;
}

void FaceIdentify::setFaceInter(FaceInterface *inter)
{
    m_interFace = inter;
}

//bool FaceIdentify::identifyExpired()
//{
//    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
//    QDateTime current_date_time =QDateTime::currentDateTime();
//    qint64 now = origin_time.msecsTo(current_date_time);
//    if(now >= m_endTimerMs)
//    {
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}

//void FaceIdentify::identifyCountdown_ms(int ms)
//{
//    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
//    QDateTime current_date_time =QDateTime::currentDateTime();
//    qint64 now = origin_time.msecsTo(current_date_time);
//    m_endTimerMs = now + ms;
//}
