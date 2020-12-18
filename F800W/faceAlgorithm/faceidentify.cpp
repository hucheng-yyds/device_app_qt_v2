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
    identifyCountdown_ms(0);
    m_tempResult = 0;
    m_irImage = new unsigned char[VIDEO_WIDTH*VIDEO_HEIGHT* 3 / 2];
    m_bgrImage = new unsigned char[VIDEO_WIDTH*VIDEO_HEIGHT* 3 / 2];
}

void FaceIdentify::setTempResult(const QString &tempVal, int result)
{
    m_tempFlag = true;
    m_tempVal = tempVal;
    m_tempResult = result;
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
        m_interFace->m_mutex.lock();
        detect((const char *)image.data, image.cols, image.rows, BGR, 0.75, image.cols/19, &faceHandle, &count);
        m_interFace->m_mutex.unlock();
        if(count > 0)
        {
            m_interFace->m_mutex.lock();
            extract(faceHandle[0], &featureFrist, &size);
            compare(featureFrist, feature_result, size, &result);
            m_interFace->m_mutex.unlock();
            QString name = dataShare->m_idCardDatas.at(0);
            if (result > switchCtl->m_idcardValue)
            {
                idPass = true;
                emit idCardResultShow(2, name, tr("认证通过"), m_faceInfo + name);
                hardware->playSound(tr("认证通过").toUtf8(), "chengong.aac");
            }
            else
            {
                idPass = false;
                emit idCardResultShow(1, name, tr("认证失败"), name);
                hardware->playSound(tr("认证失败").toUtf8(), "rzshibai.aac");
            }
            releaseFeature(featureFrist);
            releaseAllFace(faceHandle, count);
        }
    }
    return idPass;
}

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
    while (true)
    {
        g_usedSpace.acquire();
        bool ir = switchCtl->m_ir;
        bool faceDoor = switchCtl->m_faceDoorCtl;
        bool tempCtl = switchCtl->m_tempCtl;
        bool vi = switchCtl->m_vi;
        QString openMode = switchCtl->m_openMode;
        int mask = switchCtl->m_mask;
        int helmet = switchCtl->m_helet;
        if(tempCtl)
        {
            if(identifyExpired())
            {
                m_cardWork = false;
            }
            m_tempFlag = false;
            emit startTemp();
        }
        FaceHandle *bgrHandle = m_interFace->m_iFaceHandle, *irHandle;
        FacePoseBlur pose_blur;
        FaceAttr face_attr;
        QStringList datas;
        bool authority = false;
        bool egPass = false;
        int openDoorType = 1;
        bool tempPass = false;
        QString isSuccess = "0";
        QString isStranger = "0";
        QString invalidReason = "";
        QString realName = "", cardNum = "", nation = "", addr = "", birth = "";
        uint64_t face_id = 0;
        QString uploadTime = "";
        int offlineNmae = 0;
        QString cardNo = "";
        int isOver = 0;
        int sex = 0;
        QString snapshot = "";
        bool idCardResult = false;
        char *feature_result = nullptr;
        QString uplaodTemp = "";
        int bgrLength = m_interFace->m_count, irLength;
        memcpy(m_bgrImage, m_interFace->m_bgrImage, VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2);

        if (faceDoor && !m_cardWork)
        {
            if(dataShare->getReadingStatus() && dataShare->m_idCardFlag)
            {
                emit readIcStatus(3);
            }
            judgeDate();
            int i = 0;
            if(ir)
            {
                memcpy(m_irImage, m_interFace->m_irImage, VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2);
                m_interFace->m_mutex.lock();
                detect((const char *)m_irImage, VIDEO_WIDTH, VIDEO_HEIGHT, NV21, 0.75, 67, &irHandle, &irLength);
                m_interFace->m_mutex.unlock();
                QVector<int> matchPair(bgrLength, irLength);
                BGR_IR_match(bgrHandle, bgrLength, irHandle, irLength, matchPair.data());
                if (ir && matchPair[m_interFace->m_faceHandle[i].index] == irLength)
                {
                    qDebug("[FACEPASS_DV300_TEST]  This bgrIndex=%d face handle is attack, can not pass the ir filter process!!", i);
                    m_interFace->m_iStop = true;
                    goto endIdentify;
                }
                else
                {
                    if (ir && m_interFace->m_quality)
                    {
                        float liveness_result = 0.0;
                        getLiveness_bgrir(bgrHandle[m_interFace->m_faceHandle[i].index], irHandle[matchPair[m_interFace->m_faceHandle[i].index]],&liveness_result);
                        if (liveness_result < 0.8)
                        {
                            m_interFace->m_iStop = true;
                        }
                    }
                }
            }
            m_interFace->m_quality = true;
            getPoseBlurAttribute(bgrHandle[m_interFace->m_faceHandle[i].index], &pose_blur);
            if (qAbs(pose_blur.blur) > 0.8)
            {
//                    emit blur();
                m_interFace->m_quality = false;
            }
            if (qAbs(pose_blur.pitch) >= 45 || qAbs(pose_blur.roll) >= 45 || qAbs(pose_blur.yaw) >= 45)
            {
//                    emit pose();
                m_interFace->m_quality = false;
            }
            if (!m_interFace->m_iStop && m_interFace->m_quality)
            {
                if (mask)
                {
                    getFaceAttrResult(bgrHandle[m_interFace->m_faceHandle[0].index],  &face_attr);
                    qt_debug() << "mask" <<  face_attr.respirator[0] << face_attr.respirator[1] << face_attr.respirator[2];
                    if (face_attr.respirator[0] > 0.8)
                    {
                        emit maskHelmet(0);
                        hardware->playSound(tr("请戴口罩").toUtf8(), "dkouzhao.aac");
                        if (2 == mask)
                        {
                            sleep(2);
                            goto endIdentify;
                        }
                        msleep(500);
                    }
                    else {

                    }
                }
                if (helmet)
                {
                    getFaceAttrResult(bgrHandle[m_interFace->m_faceHandle[0].index],  &face_attr);
                    qt_debug() << "helmet" <<  face_attr.hat[0] << face_attr.hat[1] << face_attr.hat[2] << face_attr.hat[3] << face_attr.hat[4] << face_attr.hat[5] << face_attr.hat[6] << face_attr.hat[7] << face_attr.hat[8];
                    if (face_attr.hat[0] > 0.8 || face_attr.hat[1] < 0.2)
                    {
                        emit maskHelmet(1);
                        hardware->playSound(tr("请戴安全帽").toUtf8(), "helmet.aac");
                        if (2 == helmet)
                        {
                            sleep(2);
                            goto endIdentify;
                        }
                        msleep(500);
                    }
                }
                float result = 0.0;
                int size = 0;
                m_interFace->m_mutex.lock();
                extract(bgrHandle[m_interFace->m_faceHandle[i].index], &feature_result, &size);
                identifyFromFaceGroup(sqlDatabase->m_groupHandle, feature_result, size, &result, &face_id);
                m_interFace->m_mutex.unlock();
                if ((result > dataShare->m_faceThreshold) && (face_id > 0))
                {
                    QStringList value = dealOpencondition(face_id);
                    QString name = value.at(0);
                    QString remarks = value.at(1);
                    if(name.isEmpty())
                    {
                        authority = true;
                        emit faceResultShow(tr("未授权"), i, m_interFace->m_faceHandle[i].track_id, tr("请联系管理员"), tr("未授权"));
                        if(remarks.isEmpty())
                        {
                            hardware->playSound(tr("未授权").toUtf8(), "authority.aac");
                        }
                        else {
                            emit remark(remarks);
                            hardware->playSound(remarks.toUtf8(), "authority.aac");
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
                        emit faceResultShow(name, i, m_interFace->m_faceHandle[i].track_id, tr("认证通过"), m_faceInfo + name);
                        egPass = true;
                    }
                }
                else
                {
                    if(vi)
                    {
                        if(!dataShare->getReadingStatus())
                        {
                            if(dataShare->m_idCardFlag)
                            {
                                openDoorType = 4;
                                realName = dataShare->m_idCardDatas.at(0);
                                cardNum = dataShare->m_idCardDatas.at(1);
                                nation = dataShare->m_idCardDatas.at(2);
                                addr = dataShare->m_idCardDatas.at(3);
                                birth = dataShare->m_idCardDatas.at(4);
                                sex = dataShare->m_idCardDatas.at(5).compare("男") == 0 ? 1 : 2;
                                idCardResult = idCardFaceComparison(feature_result);
                                dataShare->m_idCardFlag = false;
                            }
                            else
                            {
                                emit idCardResultShow(0, tr("未注册"), tr("请刷身份证"), tr("请刷身份证"));
                                hardware->playSound(tr("请刷身份证").toUtf8(), "shenfenzh.aac");
                                msleep(500);
                                goto endIdentify;
                            }
                        }
                        else
                        {
                            goto endIdentify;
                        }
                    }
                    else {
                        isStranger = "1";
                        emit faceResultShow(tr("未注册"), i, m_interFace->m_faceHandle[i].track_id, tr("请联系管理员"), tr("未注册"));
                        egPass = false;
                        face_id = 0;
                    }
                }
            }
            else
            {
                goto endIdentify;
            }
            if (!tempCtl && (!vi || egPass))
            {
                if(!authority)
                {
                    if (egPass)
                    {
                        hardware->playSound(tr("认证通过").toUtf8(), "chengong.aac");
                    }
                    else {
                        hardware->playSound(tr("未注册").toUtf8(), "shibai.aac");
                    }
                }
            }
            cardNo = sqlDatabase->sqlSelectIc(face_id);
        }
        else if(m_cardWork) {
            cardNo = m_cardNo;
            int mid = sqlDatabase->sqlSelectIcId(cardNo);
            if(mid > 0)
            {
                egPass = true;
                isStranger = "0";
            }
            else {
                egPass = false;
                isStranger = "1";
            }
        }
        if(tempCtl && (m_cardWork || egPass || !vi || (vi && idCardResult) || 0 == openMode.compare("Temp")))
        {
            emit showStartTemp();
            if(faceDoor && !m_cardWork)
            {
                if(!authority && (!vi || (vi && egPass)))
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
            uplaodTemp = m_tempVal;
            if(switchCtl->m_fahrenheit)
            {
                uplaodTemp = QString("%1").arg((m_tempVal.toFloat() - 32)/1.8);
                emit tempShow(QString("%1℉").arg(m_tempVal), m_tempResult);
            }
            else
            {
                emit tempShow(QString("%1℃").arg(m_tempVal), m_tempResult);
            }
//            dataShare->m_tempFlag = false;
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
            if((egPass || idCardResult) && tempPass)
            {
                emit wgOut(cardNo.toUtf8());
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
            openDoorType = 5;
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
        else if(openMode.compare("Face") == 0 || !m_cardWork)
        {
            if(egPass || idCardResult)
            {
                isSuccess = "1";
                emit wgOut(cardNo.toUtf8());
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
        isOver = tempPass ? 0 : 1;
        datas.clear();
        uploadTime = QDateTime::currentDateTime().addSecs(28800).toString("yyyy-MM-dd HH:mm:ss");
        if(4 == openDoorType)
        {
            datas << uploadTime << uplaodTemp << isSuccess << invalidReason << isStranger << m_cardNo << realName << cardNum << nation
                  << addr << birth;
        }
        else {
            datas << uploadTime << uplaodTemp << isSuccess << invalidReason << isStranger << m_cardNo << "" << "" << "" << "" << "";
        }
        if((switchCtl->m_uploadStrangerCtl || egPass) && !vi)
        {
            if(m_cardWork && !tempCtl)
            {
            }
            else {
                if(dataShare->m_netStatus)
                {
                    emit uploadopenlog(offlineNmae, face_id, snapshot, isOver, 1, tempCtl, sex, datas);
                }
                sqlDatabase->sqlInsertOffline(offlineNmae, face_id, 1, isOver, tempCtl, sex, datas);
            }
        }
        m_cardWork = false;
        m_cardNo = "";

endIdentify:
        if(feature_result)
        {
            releaseFeature(feature_result);
        }
        if(bgrHandle)
        {
            releaseAllFace(bgrHandle, bgrLength);
        }
        if (ir && irHandle)
        {
            releaseAllFace(irHandle, irLength);
        }
        m_interFace->m_iFaceHandle = nullptr;
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
    m_cardNo = cardNo;
    bool tempCtl = switchCtl->m_tempCtl;
    if(tempCtl)
    {
        identifyCountdown_ms(3*1000);
    }
    QString isSuccess = "0";
    QString isStranger = "0";
    if(mid > 0)
    {
        judgeDate();
        QStringList value = dealOpencondition(mid);
        QString name = value.at(0);
        QString remark = value.at(1);
        if(name.isEmpty())
        {
            emit icResultShow(0, tr("未授权"), tr("未授权"));
            if(remark.isEmpty())
            {
                hardware->playSound(tr("未授权").toUtf8(), "authority.aac");
            }
            else {
                hardware->playSound(remark.toUtf8(), "authority.aac");
            }
            isSuccess = "0";
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
            if(tempCtl)
            {
                hardware->playSound(tr("请看摄像头").toUtf8(), "kansxt.aac");
            }
            else {
                hardware->playSound(tr("认证通过").toUtf8(), "chengong.aac");
                emit wgOut(cardNo.toUtf8());
                hardware->ctlLed(GREEN);
                hardware->checkOpenDoor();
                isSuccess = "1";
            }
        }
        isStranger = "0";
    }
    else {
        QString icShow = cardNo;
        if(!switchCtl->m_showIc)
        {
            icShow = tr("未注册");
        }
        emit icResultShow(0, icShow, icShow);
        if(tempCtl)
        {
            hardware->playSound(tr("请看摄像头").toUtf8(), "kansxt.aac");
        }
        else {
            hardware->playSound(tr("未注册").toUtf8(), "shibai.aac");
            hardware->ctlLed(RED);
            isSuccess = "0";
            isStranger = "1";
        }
    }
    if(!tempCtl)
    {
        QStringList datas;
        datas.clear();
        int offlineNmae = QDateTime::currentDateTime().toTime_t();
        datas << QDateTime::currentDateTime().addSecs(28800).toString("yyyy-MM-dd HH:mm:ss") << "" << isSuccess << "" << isStranger
              << cardNo << "" << "" << "" << "" << "";
        emit uploadopenlog(offlineNmae, mid, "", 0, 3, 0, 0, datas);
        sqlDatabase->sqlInsertOffline(offlineNmae, mid, 3, 0, 0, 0, datas);
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
//            qt_debug() << current_week;
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
//                qt_debug() << timeStr << timeList;
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

bool FaceIdentify::identifyExpired()
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

void FaceIdentify::identifyCountdown_ms(int ms)
{
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time =QDateTime::currentDateTime();
    qint64 now = origin_time.msecsTo(current_date_time);
    m_endTimerMs = now + ms;
}
