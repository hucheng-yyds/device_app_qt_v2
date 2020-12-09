#include "facemanager.h"
#include "datashare.h"
#include "zbarQRTest.h"

QSemaphore g_usedSpace(0);

FaceManager::FaceManager()
{
    m_isIdentify = false;
    faceCountdown_ms(0);
}

void FaceManager::updateIdentifyValue()
{
    int count = sqlDatabase->m_localFaceSet.size();
    if (0 == count || count < 1000)
    {
        dataShare->m_faceThreshold = 61;
    }
    else if (1000 <= count && count < 5000)
    {
        dataShare->m_faceThreshold = 65;
    }
    else if (5000 <= count && count < 10000)
    {
        dataShare->m_faceThreshold = 66;
    }
    else if (10000 <= count && count < 20000)
    {
        dataShare->m_faceThreshold = 68;
    }
    else if (20000 <= count && count < 50000)
    {
        dataShare->m_faceThreshold = 70;
    }
    else if (50000 <= count && count < 100000)
    {
        dataShare->m_faceThreshold = 71;
    }
}

void FaceManager::setFaceInter(FaceInterface *inter)
{
    m_interFace = inter;
}

void FaceManager::onBreathingLight()
{
    hardware->ctlLed(OFF);
    hardware->ctlBLN(ON);
    hardware->ctlIrWhite(OFF);

}

void FaceManager::run()
{
    bool status = false;
    int backLightCount = 0;
    FaceRect rect;
    int saveLeft[5];
    int saveTop[5];
    int saveRight[5];
    int saveBottom[5];
    int offset = 10;
    bool kansxt = false;
    unsigned char *rcodeImage = new unsigned char[VIDEO_WIDTH*VIDEO_HEIGHT* 3 / 2];
    onBreathingLight();
    while(true)
    {
        int rcode = switchCtl->m_rcode;
        if(dataShare->m_sync || dataShare->m_upgrade)
        {
            status = true;
            hardware->ctlWDG();
            QString text = tr("正在同步中...");
            if(dataShare->m_upgrade)
            {
                text = tr("正在升级中...");
            }
            emit faceTb(text);
            hardware->ctlLed(OFF);
            if(!dataShare->m_netStatus)
            {
                dataShare->m_sync = false;
            }
            hardware->checkOpenDoor();
            hardware->ctlWDG();
            msleep(500);
            continue;
        }
        if(status)
        {
            status = false;
            emit faceTb("");
        }
        bool ir = switchCtl->m_ir;
        m_bgrVideoFrame = nullptr;
        IF_GetData(&m_bgrVideoFrame, VIDEO_WIDTH, VIDEO_HEIGHT);
        if(m_bgrVideoFrame == nullptr)
        {
            qt_debug() << ("bgrVideoFrame error\n");
            sleep(4);
            continue;
        }
        if (ir)
        {
            m_irVideoFrame = nullptr;
            IF_GetIRData(&m_irVideoFrame, VIDEO_WIDTH, VIDEO_HEIGHT);
            if (m_irVideoFrame == nullptr)
            {
                qt_debug() << ("irVideoFrame error\n");
                sleep(4);
                continue;
            }
        }
        if(1 == rcode)
        {
            memcpy(rcodeImage, (const char *)m_bgrVideoFrame->stVFrame.u64VirAddr[0], VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2);
            cv::Mat bgr_image(VIDEO_HEIGHT * 3 / 2, VIDEO_WIDTH, CV_8UC1, rcodeImage);
            cv::Mat bgr_nv21;
            cv::resize(bgr_image, bgr_nv21, cv::Size(400, 960));
            unsigned char qrStringBuffer[256];
            int qrNumber = zbarProcessQR((unsigned char *)bgr_nv21.data, bgr_nv21.cols, bgr_nv21.rows, qrStringBuffer, 256);
            if(qrNumber > 0)
            {
                QByteArray content = QByteArray::fromBase64((const char*)qrStringBuffer);
                QByteArray datas = content.mid(0, 65);
                qt_debug() << datas;
                emit rcodeResult(datas);
            }
        }
        FaceHandle *bgrHandle;
        int bgrLength = 0;
        FaceRetCode ret_code = detectAndTrack((const char *)m_bgrVideoFrame->stVFrame.u64VirAddr[0], m_bgrVideoFrame->stVFrame.u32Stride[0],
                m_bgrVideoFrame->stVFrame.u32Height, NV21, 0.75, 67, &bgrHandle, &bgrLength);
        m_trackId.clear();
        if(RET_OK == ret_code)
        {
            m_trackId.resize(bgrLength);
            for (int i = 0; i < bgrLength; i++)
            {
                int trackId = 0;
                ret_code = getTrackId(bgrHandle[i], &trackId);
                m_trackId[i] = trackId;
            }
            sort(bgrHandle, bgrLength);
        }
        else {
            bgrLength = 0;
        }
        if (bgrLength > 0)
        {
            dataShare->m_offlineFlag = false;
            backLightCount = 0;
            hardware->ctlIrWhite(IR_WHITE);
            for(int i = 0; i < m_sMFaceHandle.size(); i++)
            {
                rect = m_sMFaceHandle[i].rect;
                offset = (rect.right - rect.left) / 20 ;
                if (((qAbs(saveLeft[i] - rect.left) > offset) ||
                     (qAbs(saveTop[i] - rect.top) > offset)) &&
                        ((qAbs(saveRight[i] - rect.right) > offset) ||
                         (qAbs(saveBottom[i] - rect.bottom) > offset))) {
                    saveLeft[i] = rect.left /*- (rect.left % offset)*/;
                    saveTop[i] = rect.top /*- (rect.top % offset)*/;
                    saveRight[i] = rect.right /*- (rect.right % offset)*/;
                    saveBottom[i] = rect.bottom /*- (rect.bottom % offset)*/;
                }
                emit showFaceFocuse(saveLeft[i], saveTop[i], saveRight[i], saveBottom[i], i, m_sMFaceHandle[i].track_id);
            }
            if (!m_interFace->m_iFaceHandle && m_isIdentify)
            {
                m_interFace->m_iFaceHandle = bgrHandle;
                int count = m_sMFaceHandle.size();
                m_interFace->m_faceHandle.resize(count);
                for(int i = 0;i < count;i++)
                {
                    m_interFace->m_faceHandle[i].index = m_sMFaceHandle[i].index;
                    m_interFace->m_faceHandle[i].track_id = m_sMFaceHandle[i].track_id;
                }
                m_interFace->m_count = bgrLength;
                m_interFace->m_iStop = false;
                memcpy(m_interFace->m_bgrImage, (const char *)m_bgrVideoFrame->stVFrame.u64VirAddr[0], VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2);
                if (ir)
                {
                    memcpy(m_interFace->m_irImage, (const char *)m_irVideoFrame->stVFrame.u64VirAddr[0], VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2);
                }
                g_usedSpace.release();
            }
            else
            {
                releaseAllFace(bgrHandle, bgrLength);
            }
        }
        else
        {
            m_interFace->m_iStop = true;
            emit hideFaceFocuse();
            if(switchCtl->m_tempCtl)
            {
                emit endTemp();
            }
            backLightCount++;
            if(backLightCount > (1 == rcode ? 30 : 100))
            {
                backLightCount = 0;
                if(!dataShare->m_offlineFlag)
                {
                    onBreathingLight();
                    dataShare->m_offlineFlag = true;
                }
            }
            releaseAllFace(bgrHandle, bgrLength);
        }
        hardware->checkCloseDoor();
        hardware->ctlWDG();
        if (ir)
        {
            IF_ReleaseIRData(m_irVideoFrame);
        }
        IF_ReleaseData(m_bgrVideoFrame);
    }
}

bool FaceManager::filter(const FaceRect &rect)
{
    bool pass = true;
    int width = rect.right - rect.left;
    if (switchCtl->m_tempCtl)
    {
        if (width > 400)
        {
            pass = false;
//            emit faceTips(tr("太近了"));
        }
        else
        {
            bool region = true;
            bool dis;
            if (switchCtl->m_loose)
            {
                dis = width > 150;
            }
            else
            {
                dis = width > 250;
//                region = rect.left > 92 && rect.top > 281 && rect.right < 708 && rect.bottom < 861;
                int x = (rect.right - rect.left)/2 + rect.left;
                int y = (rect.bottom - rect.top)/4 + rect.top;
                region = x > 330 && x < 480 &&  y > 526 && y < 646;
            }
            pass = dis && region;
            if (region && !dis)
            {
//                emit blur();
            }
        }
    }
    else
    {
        if(width <= 45)
        {
            pass = false;
//            emit blur();
        }
    }
    return pass;
}

void FaceManager::sort(FaceHandle *faceHandle, int count)
{
    int i, j;
    int waitTime = switchCtl->m_identifyWaitTime;
    MFaceHandle buf;
    FaceRect rect0 ,rect1;
    m_sMFaceHandle.clear();
    m_isIdentify = true;
    for (i = 0; i < count; i ++)
    {
        getFaceRect(faceHandle[i], &rect0);
        if (filter(rect0))
        {
            buf.rect = rect0;
            buf.track_id = m_trackId.at(i);
            buf.index = i;
            m_sMFaceHandle << buf;
        }
    }
    for (i=0; i<m_sMFaceHandle.size()-1; ++i)  //比较n-1轮
    {
        for (j=0; j<m_sMFaceHandle.size()-1-i; ++j)  //每轮比较n-1-i次,
        {
            rect0 = m_sMFaceHandle[j].rect;
            rect1 = m_sMFaceHandle[j+1].rect;
            int width0 = rect0.right - rect0.left;
            int width1 = rect1.right - rect1.left;
            if (width0 < width1)
            {
                buf = m_sMFaceHandle[j];
                m_sMFaceHandle[j] = m_sMFaceHandle[j+1];
                m_sMFaceHandle[j+1] = buf;
            }
        }
    }
    if (m_sMFaceHandle.isEmpty())
    {
        m_isIdentify = false;
    }
    m_sMFaceHandle.resize(1);
    if (m_sMFaceHandle.at(0).track_id && !m_interFace->m_iFaceHandle)
    {
        if (m_sMFaceHandle.at(0).track_id == m_interFace->m_faceHandle.at(0).track_id)
        {
            if (!faceExpired())
            {
                if (m_interFace->m_quality && !m_interFace->m_iStop)
                {
                    if(!(dataShare->m_idCardFlag && switchCtl->m_vi))
                    {
                        m_isIdentify = false;
                    }
                }
            }
            else
            {
                faceCountdown_ms(waitTime*1000);
            }
        }
        else
        {
            faceCountdown_ms(waitTime*1000);
        }
    }
}

void FaceManager::ctlOpenDoor(int id)
{
    hardware->ctlLed(GREEN);
    hardware->checkOpenDoor();
    QStringList datas;
    datas.clear();
    datas << QDateTime::currentDateTime().addSecs(28800).toString("yyyy-MM-dd HH:mm:ss") << "" << "1" << "" << "0" << "" << "" << "" << "" << "" << "";
    sqlDatabase->sqlInsertOffline(0, id, 1, 0, 0, 0, datas);
}

void FaceManager::insertFaceGroups(int id, const QString &username, const QString &time, const QString &photoname, const QString &iphone)
{
    int count;
    QString file = QString::number(id) + ".jpg";
    FaceHandle *faceHandle = nullptr;
    auto image = cv::imread(file.toStdString());
    if (image.empty() || (image.cols > 1920) || (image.rows > 1920))
    {
        int errid = 1;
        if(image.empty())
        {
            errid = 4;
        }
        sqlDatabase->sqlInsertFail(id, errid);
        qt_debug() << "Error: id" << id << errid;
    }
    else
    {
        FacePoseBlur pose_blur;
        m_interFace->m_mutex.lock();
        detect((const char *)image.data, image.cols, image.rows, BGR, 0.75, image.cols/19, &faceHandle, &count);
        m_interFace->m_mutex.unlock();
        if (0 == count)
        {
            sqlDatabase->sqlInsertFail(id,  2);
            qt_debug() << "faceHandle is null !" << id << "2";
        }
        else
        {
            getPoseBlurAttribute(faceHandle[0], &pose_blur);
            if ((qAbs(pose_blur.pitch) < 35 || qAbs(pose_blur.roll) < 35 || qAbs(pose_blur.yaw) < 35)
                               && (qAbs(pose_blur.blur) < 0.7))
            {
                char *feature_result;
                int size;
                m_interFace->m_mutex.lock();
                extract(faceHandle[0], &feature_result, &size);
                m_interFace->m_mutex.unlock();
                QStringList feature;
                for (int i = 0; i < size; i ++)
                {
                    feature << QString::number(feature_result[i]);
                }
//                qt_debug() << id << username << time << photoname << iphone;
                sqlDatabase->sqlInsert(id, username, time, feature.join(","), photoname, iphone);
                insertFaceGroup(sqlDatabase->m_groupHandle, feature_result, size, id);
                if(sqlDatabase->m_timeoutFaceFail.contains(id))
                {
                    sqlDatabase->m_timeoutFaceFail.remove(id);
                    sqlDatabase->sqlDeleteFail(id);
                }
                releaseFeature(feature_result);
            }
            else
            {
                qt_debug() << "failInsert" << 3;
                sqlDatabase->sqlInsertFail(id, 3);
            }
        }
        releaseAllFace(faceHandle, count);
    }
    QFile::remove(file);
}

bool FaceManager::init()
{
    FaceModels models;
    memset(&models, 0 , sizeof(FaceModels));
    models.anchor_path = "model/3516_anchor_A_encrypt.bin";
    models.detect_model = "model/3516_detect_G_encrypt.bin";
    models.postfilter_model = "model/3516_postfilter_F_encrypt.bin";
    models.pose_blur_model = "model/3516_poseblur_B_encrypt.bin";
    models.refine_model = "model/3516_landmark_A_encrypt.bin";
    models.liveness_bgr_model = "model/3516_live_rgb_F_encrypt.bin";
    models.liveness_bgrir_model = "model/3516_live_rgbir_F_plus_encrypt.bin";
    models.age_gender_model = "model/3516_age_gender_A_encrypt.bin";
    models.rc_model = "model/3516_rc_attr_D_encrypt.bin";
    models.occl_model = "model/3516_occlusion_A_encrypt.bin";
    models.stn_model = "model/3516_preFeat_Nemo_encrypt.bin";
    models.feature_model = "model/3516_feat_Nemo_encrypt.bin";
    models.group_model_path = "model/3516_search_B_encrypt.bin";

    setLogLevel(LOG_LEVEL_ERROR);
    int ret = ::init(models);
    if (ret != RET_OK)
    {
//        qt_debug() << "init failed !";
        return false;
    }
    set_detect_config(0.3, 0.5);
    set_match_config(0.99, -35.61, 0.99, 4.26, 0.4);
    createFaceGroup(&sqlDatabase->m_groupHandle);
    localFaceInsert();
    return true;
}

void FaceManager::localFaceInsert()
{
    QSet<int> ids = sqlDatabase->sqlSelectAllUserId();
    foreach (int id, ids)
    {
        emit faceTb(tr("加载人员中..."));
        QString value = sqlDatabase->sqlSelectAllUserFeature(id);
        if ("0" != value && !value.isEmpty())
        {
            QStringList result = value.split(",");
            QVector<char> ret;
            int size = result.size();
            ret.resize(size);
            for (int i = 0; i < size; i ++)
            {
                ret[i] = result.at(i).toFloat();
            }
            insertFaceGroup(sqlDatabase->m_groupHandle, ret.data(), ret.size(), id);
        }
        else {
            qt_debug() << "value 0" << value;
        }
    }
    emit faceTb(tr(""));
    updateIdentifyValue();
}

bool FaceManager::faceExpired()
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

void FaceManager::faceCountdown_ms(int ms)
{
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_date_time =QDateTime::currentDateTime();
    qint64 now = origin_time.msecsTo(current_date_time);
    m_endTimerMs = now + ms;
}
