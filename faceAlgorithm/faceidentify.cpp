#include "faceidentify.h"
#include <QImage>

FaceIdentify::FaceIdentify()
{
    m_interFace = nullptr;
    m_tempFlag = false;
    m_cardWork = false;
    m_tempVal = "0";
    m_tempResult = 0;
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
        m_wavSeq = 0;
        m_faceInfo = tr("早上好，");
    }
    else if(curTime.compare("12:00:00") > 0 && curTime.compare("18:30:00") < 0)
    {
        m_wavSeq = 1;
        m_faceInfo = tr("下午好，");
    }
    else
    {
        m_wavSeq = 2;
        m_faceInfo = tr("晚上好，");
    }
}

void FaceIdentify::setTempResult(const QString &tempVal, int result)
{
    m_tempFlag = true;
    m_tempVal = tempVal;
    m_tempResult = result;
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
        m_interFace->m_usedSpace.acquire();
        int i = 0;
        QVector<DS_FaceInfo> &m_iMFaceHandle = m_interFace->m_faceHandle;
        int trId = m_iMFaceHandle[i].trackID;
        bool ir = switchCtl->m_ir;
        bool faceDoor = switchCtl->m_faceDoorCtl;
        bool tempCtl = switchCtl->m_tempCtl;
        bool vi = switchCtl->m_vi;
        QString openMode = switchCtl->m_openMode;

        QStringList datas;
        bool authority = false;
        bool egPass = false;
        QString playFile = "chengong.wav";
        bool tempPass = false;
        QString isSuccess = "0";
        QString isStranger = "0";
        QString invalidReason = "";
        uint64_t face_id = (uint64_t)m_iMFaceHandle[i].ID;
        QString uploadTime = "";
        int offlineNmae = 0;
        QString tempVal = "0";
        int tempResult = 0;
        QString cardNo = "";
        int isOver = 0;
        QString snapshot = "";
        if(tempCtl)
        {
            emit startTemp();
        }
//        if (m_iMFaceHandle[i].pose > 0) {
//            qt_debug() << "m_iMFaceHandle.pose:" << m_iMFaceHandle[i].pose;
//        }
//        if (!m_iMFaceHandle[i].irLiveOrNot) {
//            qt_debug() << "m_iMFaceHandle[i].irLiveOrNot:" << m_iMFaceHandle[i].irLiveOrNot;
//        }
//        qt_debug() << "ptrFaceInfo.recStatus:" << m_iMFaceHandle[i].recStatus;
//        qt_debug() << "ptrFaceInfo.recScoreVal:" << m_iMFaceHandle[i].recScoreVal;
//        qt_debug() << "ptrFaceInfo.faceMaskOrNot:" << m_iMFaceHandle[i].faceMaskOrNot;

        if (RETURN_REC_SUCCESS == m_iMFaceHandle[i].recStatus)
        {
            m_interFace->m_success = true;
            dataShare->m_offlineFlag = false;
            /*不同人脸识别间隔判断*/
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
            judgeDate();
//            QStringList value = dealOpencondition(face_id);
            QString name = sqlDatabase->sqlSelect(face_id).value(1).toString();
//            QString remark = value.at(1);
//            if(name.isEmpty())
//            {
//                authority = true;
//                emit faceResultShow(name, i, trId, tr("未授权"), tr("未授权"));
//                if(remark.isEmpty()) {
//                    hardware->playSound("authority.aac");
//                } else {
//                    hardware->playSound("authority.aac");
//                }
//            }
//            else
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
                emit faceResultShow(name, i, trId, tr("认证通过"), tr("您好，") + name);
                egPass = true;
//                if(0 == m_wavSeq)
//                {
//                    playFile = "zschengong.wav";
//                }
//                else if(1 == m_wavSeq)
//                {
//                    playFile = "zwchengong.wav";
//                }
//                else
//                {
//                    playFile = "wschengong.wav";
//                }
                playFile = "chengong.wav";
                hardware->playSound(playFile.toUtf8());
            }
            if(tempCtl && !authority) {
                emit showStartTemp();
                qt_debug() << "holding temp" << m_tempFlag;
                int tempCount = 0;
                while (1)
                {
                    msleep(10);
                    if(m_tempFlag)
                    {
                        break;
                    }
                    tempCount++;
                    if(tempCount > 250)
                    {
                        m_tempResult = -2;
                        tempCount = 0;
                        break;
                    }
                }
                m_tempFlag = false;
                tempVal = m_tempVal;
                tempResult = m_tempResult;
                if(switchCtl->m_fahrenheit)
                {
                    emit tempShow(QString("%1℉").arg(tempVal), tempResult);
                }
                else
                {
                    emit tempShow(QString("%1℃").arg(tempVal), tempResult);
                }
                msleep(150);
                qt_debug() << tempVal << tempResult;
                float warnValue = switchCtl->m_warnValue;
                if(switchCtl->m_fahrenheit)
                {
                    warnValue = (warnValue)*1.8 + 32.0;
                }
                if (tempVal.toFloat() >= warnValue)
                {
                    tempPass = false;
                }
                else if(tempResult > 0)
                {
                    tempPass = true;
                }
                if(-1 == tempResult)
                {
                    tempPass = false;
                    hardware->playSound("tiwenlow.wav");
                }
                else if(-2 == tempResult)
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
            }
        } else if (RETURN_RECOGING == m_iMFaceHandle[i].recStatus) {
            if (m_iMFaceHandle[i].recScoreVal < 0.75 &&
                    m_iMFaceHandle[i].pose == 0 /*&&
                    identify->track_id.value(i) == trId*/) {
//                if (timer.expired()) {
//                    timer.countdown(3);
//                } else {
//                    qt_debug() << "22222222222sid:" << trId;
//                    goto exit;
//                }
                m_interFace->m_success = true;
                emit faceResultShow(tr("认证失败"), i, trId, tr("认证失败"), tr("认证失败"));
                isStranger = "1";
//                emit faceResultShow(tr("未注册"), i, trId, tr("请联系管理员"), tr("未注册"));
                egPass = false;
                face_id = 0;
                hardware->playSound("shibai.wav");
//                goto  exit;
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
        if (switchCtl->m_uploadImageCtl && m_interFace->m_bgrImage)
        {
            if (1) {
                QImage image(m_interFace->m_bgrImage, VIDEO_WIDTH, VIDEO_HEIGHT, QImage::Format_RGB888);
                image.rgbSwapped().save("snap.jpg", "JPG", 30);
            } else {
                cv::Mat nv21(VIDEO_HEIGHT + VIDEO_HEIGHT / 2, VIDEO_WIDTH, CV_8UC1, m_interFace->m_bgrImage);
                cv::Mat image;
                cv::cvtColor(nv21, image, CV_YUV2BGR_NV12);
                if(image.empty()) {
                    qDebug("load image error!!");
                    goto exit;
                }
                QVector<int> opts;
                opts.push_back(cv::IMWRITE_JPEG_QUALITY);
                opts.push_back(30);
                opts.push_back(cv::IMWRITE_JPEG_OPTIMIZE);
                opts.push_back(1);
                cv::imwrite("snap.jpg", image, opts.toStdVector());
            }
            QFile file("snap.jpg");
            file.open(QIODevice::ReadWrite);
//            file.write((const char*)m_interFace->m_bgrImage);
            snapshot = QString::fromUtf8(file.readAll().toBase64());
            file.close();
            offlineNmae = QDateTime::currentDateTime().toTime_t();
            QString offline_path = "cp snap.jpg " + dataShare->m_offlinePath + QString::number(offlineNmae) + ".jpg";
            system(offline_path.toStdString().c_str());
        }
        isOver = tempPass ? 1 : 0;
        datas.clear();
        uploadTime = QDateTime::currentDateTime().addSecs(28800).toString("yyyy-MM-dd HH:mm:ss");
        datas << uploadTime << m_tempVal << isSuccess << invalidReason << isStranger << m_cardNo << "" << "" << "" << "" << "";
        if((switchCtl->m_uploadStrangerCtl || egPass) && !vi)
        {
            if(m_cardWork && !tempCtl)
            {
            }
            else {
                qt_debug() << "上传did" << offlineNmae;
                if(dataShare->m_netStatus)
                {
                    emit uploadopenlog(offlineNmae, face_id, snapshot, isOver, 1, tempCtl, 0, datas);
                }
                sqlDatabase->sqlInsertOffline(offlineNmae, face_id, 1, isOver, tempCtl, 0, datas);
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
    if(cardNo.isEmpty())
    {
        m_cardWork = false;
    }
    else {
        m_cardWork = true;
    }
    int id = mid;
    m_cardNo = cardNo;
    QString isSuccess = "0";
    QString isStranger = "0";
    QString playFile = "chengong.wav";
    if(id > 0)
    {
//        QStringList value = dealOpencondition(id);
        QString name = sqlDatabase->sqlSelect(id).value(1).toString();
//        QString remark = value.at(1);
//        if(name.isEmpty())
//        {
//            emit icResultShow(1, tr("未授权"), tr("未授权"));
//            if(remark.isEmpty())
//            {
//                hardware->playSound("authority.wav");
//            }
//            else {
//                hardware->playSound("authority.wav");
//            }
//            isSuccess = "0";
//        }
//        else
        {
            judgeDate();
            if(0 == switchCtl->m_language)
            {
                if(1 == switchCtl->m_nameMask)
                {
                    if(name.size() > 3)
                    {
                        name = name.replace(0, 2, "**");
                    }
                    else if(name.size() > 1)
                    {
                        name = name.replace(0, 1, '*');
                    }
                }
                else if(2 == switchCtl->m_nameMask)
                {
                    name = name.replace(0, name.size(), tr("您好"));
                }
            }
            emit icResultShow(1, name, m_faceInfo + name);
            if(0 == m_wavSeq)
            {
                playFile = "zschengong.wav";
            }
            else if(1 == m_wavSeq)
            {
                playFile = "zwchengong.wav";
            }
            else
            {
                playFile = "wschengong.wav";
            }
            hardware->playSound(playFile.toUtf8());
            hardware->checkOpenDoor();
            isSuccess = "1";
        }
        isStranger = "0";
    }
    else {
        id = 0;
        QString icShow = cardNo;
        if(!switchCtl->m_showIc)
        {
            icShow = tr("未注册");
        }
        emit icResultShow(0, icShow, icShow);
        hardware->playSound("shibai.wav");
        isSuccess = "0";
        isStranger = "1";
    }
    if(1)
    {
        QStringList datas;
        datas.clear();
        int offlineNmae = QDateTime::currentDateTime().toTime_t();
        datas << QDateTime::currentDateTime().addSecs(28800).toString("yyyy-MM-dd HH:mm:ss") << ""
              << isSuccess << "" << isStranger << cardNo << "" << "" << "" << "" << "";
        if(dataShare->m_netStatus)
        {
            emit uploadopenlog(offlineNmae, id, "", 0, 3, 0, 0, datas);
        }
        sqlDatabase->sqlInsertOffline(offlineNmae, id, 3, 0, 0, 0, datas);
        m_cardWork = false;
        m_cardNo = "";
    }
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
//    qt_debug() << value;
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
