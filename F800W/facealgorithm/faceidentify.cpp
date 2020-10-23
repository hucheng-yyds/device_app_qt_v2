#include "faceidentify.h"

extern QSemaphore g_usedSpace;

FaceIdentify::FaceIdentify()
{
    m_interFace = nullptr;
    m_irImage = new unsigned char[VIDEO_WIDTH*VIDEO_HEIGHT* 3 / 2];
    m_bgrImage = new unsigned char[VIDEO_WIDTH*VIDEO_HEIGHT* 3 / 2];
}

void FaceIdentify::run()
{
    while (true)
    {
        g_usedSpace.acquire();
        FaceHandle *bgrHandle = m_interFace->m_iFaceHandle, *irHandle;
        m_iMFaceHandle = m_interFace->m_faceHandle;
        FacePoseBlur pose_blur;
        FaceAttr face_attr;
        bool authority = false;
        bool egPass = false;
        char *feature_result = nullptr;
        int bgrLength = m_interFace->m_count,irLength;
        memcpy(m_bgrImage, m_interFace->m_bgrImage, VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2);
        memcpy(m_irImage, m_interFace->m_irImage, VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2);
        m_interFace->m_mutex.lock();
        detect((const char *)m_irImage, VIDEO_WIDTH, VIDEO_HEIGHT, NV21, 0.75, &irHandle, &irLength);
        m_interFace->m_mutex.unlock();
        QVector<int> matchPair(bgrLength, irLength);
        BGR_IR_match(bgrHandle, bgrLength, irHandle, irLength, matchPair.data());
        if (1)
        {
            int i = 0;
            if (matchPair[m_iMFaceHandle[i].index] == irLength /*&& settings->isIr*/)
            {
                qDebug("[FACEPASS_DV300_TEST]  This bgrIndex=%d face handle is attack, can not pass the ir filter process!!", i);
                m_interFace->m_iStop = true;
                goto endIdentify ;
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
                if (/*settings->isIr &&*/ m_interFace->m_quality)
                {
                    float liveness_result = 0.0;
                    getLiveness_bgrir(bgrHandle[m_iMFaceHandle[i].index], irHandle[matchPair[m_iMFaceHandle[i].index]],&liveness_result);
                    if (liveness_result < 0.8)
                    {
//                        emit identifySuccess("", i, iMFaceHandle[i].track_id);
                        m_interFace->m_iStop = true;
//                        qt_debug() << "liveness_result: " << liveness_result;
                    }
                }
                if (!m_interFace->m_iStop && m_interFace->m_quality)
                {
                    float result = 0.0;
                    int size = 0;
                    uint64_t face_id = 0;
                    m_interFace->m_mutex.lock();
                    extract(bgrHandle[m_iMFaceHandle[i].index], &feature_result, &size);
                    identifyFromFaceGroup(m_interFace->m_groupHandle, feature_result, size, &result, &face_id);
                    m_interFace->m_mutex.unlock();
                    if ((result > 60) && (face_id > 0))
                    {
//                        qt_debug() << "result" << result << "face_id" << face_id;
                        QStringList value = dealOpencondition(face_id);
                        QString name = value.at(0);
                        QString remark = value.at(1);
                        if(name.isEmpty())
                        {
//                            failReason = "no authority";
                            authority = true;
//                            emit onlineRemark(true, tr("没有权限"), remark);
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
//                            emit onlineRemark(false, "", remark);
//                            emit wgOut(1, sqlDatabase->sqlICSelect(face_id, "mid").value(0).toByteArray());
//                            if(settings->language == 0)
//                            {
//                                if(1 == settings->isNameMask)
//                                {
//                                    if(name.size() > 3)
//                                    {
//                                        name = name.replace(0, 2, "**");
//                                    }
//                                    else if(name.size() > 1){
//                                        name = name.replace(0, 1, '*');
//                                    }
//                                }
//                                else if(2 == settings->isNameMask)
//                                {
//                                    name = name.replace(0, name.size(), tr("您好"));
//                                }
//                            }
                            emit faceResultShow(name, i, m_iMFaceHandle[i].track_id, tr("认证通过"));
                            egPass = true;
                        }
//                        isStranger = "0";
                    } else {
                        QString result = tr("请联系管理员");
                        emit faceResultShow(tr("未注册"), i, m_iMFaceHandle[i].track_id, result);
                        egPass = false;
                        face_id = 0;
                    }
                }
                else
                {
                    goto endIdentify ;
                }
            }
            if(!authority)
            {
                if (egPass) {
                    hardware->playSound(tr("认证通过").toUtf8(), "chengong.aac");
                    //                    hardware->playSound(name.toUtf8());
                } else {
                    hardware->playSound(tr("未注册").toUtf8(), "shibai.aac");
                }
            }
        }
        if (1)
        {
            cv::Mat nv21(VIDEO_HEIGHT + VIDEO_HEIGHT / 2, VIDEO_WIDTH, CV_8UC1, m_bgrImage);
            cv::Mat image;
            cv::cvtColor(nv21, image, CV_YUV2BGR_NV21);
            if(image.empty()) {
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
            QString snapshot = QString::fromUtf8(file.readAll().toBase64());
            file.close();
        }
endIdentify:
        if(feature_result)
        {
            releaseFeature(feature_result);
        }
        releaseAllFace(bgrHandle, bgrLength);
        if (1) {
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
    QVariantList value = sqlDatabase->sqlSelect(faceId);
    if(value.size() <= 0)
    {
        text << "" << "";
        return text;
    }
    int passnum = value[4].toInt();
    QString startTime = value[5].toString();
    QString expireTime = value[6].toString();
    int isBlack = value[7].toInt();
    QString passPeriod = value[8].toString();
    QString passTimeSection = value[9].toString();
    QString remark = "";
    qDebug() << "========" << startTime;
    if(1 == isBlack)
    {
        remark = value[12].toString();
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
    qDebug() << pass;
    if(pass)
    {
        pass = false;
        qDebug() << "----" << passTimeSection;
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
    qDebug() << pass;
    if(pass)
    {
        name = value[1].toString();
        if(passnum >= 1)
        {
            sqlDatabase->sqlUpdatePass(faceId, passnum-1);
        }
    }
    remark = value[12].toString();
    text << name << remark;
    return text;
}

void FaceIdentify::setFaceInter(FaceInterface *inter)
{
    m_interFace = inter;
}
