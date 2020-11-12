#include "faceidentify.h"

extern QSemaphore g_usedSpace;

FaceIdentify::FaceIdentify()
{
    m_interFace = nullptr;
    m_tempFlag = false;
    m_tempVal = "0";
    m_tempResult = 0;
    m_irImage = new unsigned char[VIDEO_WIDTH*VIDEO_HEIGHT* 3 / 2];
    m_bgrImage = new unsigned char[VIDEO_WIDTH*VIDEO_HEIGHT* 3 / 2];
}

bool FaceIdentify::idCardFaceComparison(char *feature_result)
{
    int size = 0;
    float result = 0;
    QString file = "sfz.bmp";
    char *featureFrist;
    FaceHandle *faceHandle = nullptr;
    int count = 0;
    bool idPass = false;
    auto image = cv::imread(file.toStdString());
    if(!image.empty())
    {
        FacePoseBlur pose_blur;
        detect((const char *)image.data, image.cols, image.rows, BGR, 0.75, &faceHandle, &count);
        if(count > 0)
        {
            extract(faceHandle[0], &featureFrist, &size);
            compare(featureFrist, feature_result, size, &result);
            QString name = switchCtl->m_idCardDatas.at(0);
            if (result > switchCtl->m_idcardValue)
            {
                idPass = true;
                emit faceResultShow(name, 0, m_iMFaceHandle[0].track_id, tr("认证通过"));
                hardware->playSound(tr("认证通过").toUtf8(), "chengong.aac");
            }
            else
            {
                idPass = false;
                emit faceResultShow(name, 0, m_iMFaceHandle[0].track_id, tr("人证失败"));
                hardware->playSound(tr("认证通过").toUtf8(), "rzshibai.aac");
            }
            releaseFeature(featureFrist);
            releaseAllFace(faceHandle, count);
        }
    }
    return idPass;
}

void FaceIdentify::run()
{
    while (true)
    {
        g_usedSpace.acquire();
        bool ir = switchCtl->m_ir;
        bool faceDoor = switchCtl->m_faceDoorCtl;
        bool tempCtl = switchCtl->m_tempCtl;
        bool vi = switchCtl->m_vi;
        QString openMode = switchCtl->m_openMode;
        if(tempCtl)
        {
            m_tempFlag = false;
            emit startTemp();
        }
        FaceHandle *bgrHandle = m_interFace->m_iFaceHandle, *irHandle;
        m_iMFaceHandle = m_interFace->m_faceHandle;
        FacePoseBlur pose_blur;
        FaceAttr face_attr;
        QStringList datas;
        bool authority = false;
        bool egPass = false;
        bool tempPass = false;
        QString isSuccess = "0";
        QString isStranger = "0";
        QString invalidReason = "";
        uint64_t face_id = 0;
        QString uploadTime = "";
        int offlineNmae = 0;
        int isOver = 0;
        QString snapshot = "";
        bool idCardResult = false;
        char *feature_result = nullptr;
        int bgrLength = m_interFace->m_count, irLength;
        memcpy(m_bgrImage, m_interFace->m_bgrImage, VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2);
        memcpy(m_irImage, m_interFace->m_irImage, VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2);
        m_interFace->m_mutex.lock();
        detect((const char *)m_irImage, VIDEO_WIDTH, VIDEO_HEIGHT, NV21, 0.75, &irHandle, &irLength);
        m_interFace->m_mutex.unlock();
        QVector<int> matchPair(bgrLength, irLength);
        BGR_IR_match(bgrHandle, bgrLength, irHandle, irLength, matchPair.data());
        if (faceDoor)
        {
            int i = 0;
            if (matchPair[m_iMFaceHandle[i].index] == irLength && ir)
            {
                qDebug("[FACEPASS_DV300_TEST]  This bgrIndex=%d face handle is attack, can not pass the ir filter process!!", i);
                m_interFace->m_iStop = true;
                goto endIdentify;
            }
            else
            {
                m_interFace->m_quality = true;
                getPoseBlurAttribute(bgrHandle[m_iMFaceHandle[i].index], &pose_blur);
                if (qAbs(pose_blur.blur) > 0.5)
                {
//                    emit blur();
                    m_interFace->m_quality = false;
                }
                if (qAbs(pose_blur.pitch) > 20 || qAbs(pose_blur.roll) > 20 || qAbs(pose_blur.yaw) > 20)
                {
//                    emit pose();
                    m_interFace->m_quality = false;
                }
                if (ir && m_interFace->m_quality)
                {
                    float liveness_result = 0.0;
                    getLiveness_bgrir(bgrHandle[m_iMFaceHandle[i].index], irHandle[matchPair[m_iMFaceHandle[i].index]],&liveness_result);
                    if (liveness_result < 0.8)
                    {
                        m_interFace->m_iStop = true;
                    }
                }
                if (!m_interFace->m_iStop && m_interFace->m_quality)
                {
                    float result = 0.0;
                    int size = 0;
                    m_interFace->m_mutex.lock();
                    extract(bgrHandle[m_iMFaceHandle[i].index], &feature_result, &size);
                    identifyFromFaceGroup(sqlDatabase->m_groupHandle, feature_result, size, &result, &face_id);
                    m_interFace->m_mutex.unlock();
                    if ((result > switchCtl->m_faceThreshold) && (face_id > 0))
                    {
                        QStringList value = dealOpencondition(face_id);
                        QString name = value.at(0);
                        QString remark = value.at(1);
                        if(name.isEmpty())
                        {
                            authority = true;
                            if(remark.isEmpty())
                            {
                                hardware->playSound(tr("未授权").toUtf8(), "authority.aac");
                            }
                            else {
                                hardware->playSound(remark.toUtf8(), "authority.aac");
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
                            emit faceResultShow(name, i, m_iMFaceHandle[i].track_id, tr("认证通过"));
                            egPass = true;
                        }
                    }
                    else
                    {
                        if(vi)
                        {
                            if(switchCtl->m_idCardFlag)
                            {
                                idCardResult = idCardFaceComparison(feature_result);
                                switchCtl->m_idCardFlag = false;
                            }
                            else {
                                emit faceResultShow(tr("未注册"), i, m_iMFaceHandle[i].track_id, tr("请刷身份证"));
                                hardware->playSound(tr("请刷身份证").toUtf8(), "shenfenzh.aac");
                                goto endIdentify;
                            }
                        }
                        else {
                            isStranger = "1";
                            QString result = tr("请联系管理员");
                            emit faceResultShow(tr("未注册"), i, m_iMFaceHandle[i].track_id, result);
                            egPass = false;
                            face_id = 0;
                        }
                    }
                }
                else
                {
                    goto endIdentify;
                }
            }
            if (!tempCtl && !vi)
            {
                if(!authority)
                {
                    if (egPass) {
                        hardware->playSound(tr("认证通过").toUtf8(), "chengong.aac");
                    } else {
                        hardware->playSound(tr("未注册").toUtf8(), "shibai.aac");
                    }
                }
            }
        }
        if(tempCtl && (egPass || idCardResult || 0 == openMode.compare("Temp")))
        {
            emit showStartTemp();
            if(faceDoor)
            {
                if(!authority && !vi)
                {
                    if (egPass)
                    {
                        hardware->playSound(tr("认证通过").toUtf8(), "chengong.aac");
                    }
                    else
                    {
                        hardware->playSound(tr("未注册").toUtf8(), "shibai.aac");
                    }
                }
            }
            qt_debug() << "holding temp" << m_tempFlag;
            while (1)
            {
                msleep(10);
                if(switchCtl->m_tempFlag)
                {
                    break;
                }
            }
            m_tempVal = switchCtl->m_tempVal;
            m_tempResult = switchCtl->m_tempResult;
            if(switchCtl->m_fahrenheit)
            {
                emit tempShow(QString("%1℉").arg(m_tempVal), m_tempResult);
            }
            else
            {
                emit tempShow(QString("%1℃").arg(m_tempVal), m_tempResult);
            }
            switchCtl->m_tempFlag = false;
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
                hardware->playSound(tr("体温偏低").toUtf8(), "tiwenlow.aac");
            }
            else if(-2 == m_tempResult)
            {
                tempPass = false;
                hardware->playSound(tr("测温失败").toUtf8(), "cwshibai.aac");
            }
            else
            {
                if (2 == switchCtl->m_tempValueBroadcast && !switchCtl->m_tts)
                {
                    hardware->playSoundTTS(m_tempVal.toUtf8() + tr("度").toUtf8());
                }
                else
                {
                    if (tempPass)
                    {
                        hardware->playSound(tr("体温正常").toUtf8(), "tiwenzhc.aac");
                    }
                    else
                    {
                        hardware->playSound(tr("体温异常").toUtf8(), "tiwenyc.aac");
                    }
                }
            }
        }
        if(openMode.compare("FaceTemp") == 0)
        {
            if((egPass && tempPass) || idCardResult)
            {
                hardware->ctlLed(GREEN);
                hardware->checkOpenDoor();
                isSuccess = "1";
            }
            else
            {
                hardware->ctlLed(RED);
                isSuccess = "0";
            }
        }
        else if(openMode.compare("Temp") == 0)
        {
            if(tempPass)
            {
                isSuccess = "1";
                hardware->ctlLed(GREEN);
                hardware->checkOpenDoor();
            }
            else {
                isSuccess = "0";
                hardware->ctlLed(RED);
            }
        }
        else if(openMode.compare("Face") == 0)
        {
            if(egPass || idCardResult)
            {
                isSuccess = "1";
                hardware->ctlLed(GREEN);
                hardware->checkOpenDoor();
            }
            else {
                isSuccess = "0";
                hardware->ctlLed(RED);
            }
        }
        if (switchCtl->m_uploadImageCtl)
        {
            cv::Mat nv21(VIDEO_HEIGHT + VIDEO_HEIGHT / 2, VIDEO_WIDTH, CV_8UC1, m_bgrImage);
            cv::Mat image;
            cv::cvtColor(nv21, image, CV_YUV2BGR_NV21);
            if(image.empty())
            {
                printf("load image error!!\n");
                goto endIdentify;
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
        datas << uploadTime << m_tempVal << isSuccess << invalidReason << isStranger << "";
        if(!switchCtl->m_uploadStrangerCtl && !vi)
        {
            if(switchCtl->m_netStatus)
            {
                emit uploadopenlog(offlineNmae, face_id, snapshot, isOver, 1, tempCtl, datas);
            }
            sqlDatabase->sqlInsertOffline(offlineNmae, face_id, 1, isOver, tempCtl, datas);
        }

endIdentify:
        if(feature_result)
        {
            releaseFeature(feature_result);
        }
        releaseAllFace(bgrHandle, bgrLength);
        if (ir)
        {
            releaseAllFace(irHandle, irLength);
        }
        m_interFace->m_iFaceHandle = nullptr;
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
