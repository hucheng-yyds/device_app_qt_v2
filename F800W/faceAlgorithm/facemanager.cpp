#include "facemanager.h"
#include "datashare.h"
#include "zbarQRTest.h"

QSemaphore g_usedSpace(0);

FaceManager::FaceManager()
{
    m_isIdentify = false;
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

static ushort CRC16_CCITT(QByteArray content)
{
    content.replace(",", "");
    content.remove(content.size() - 4, 4);
    content = QByteArray::fromHex(content);
    content.append(0x01);
    content.append(0x02);
    content.append(0x03);
    content.append(0x04);
    const uchar *nData = (const uchar*)content.data();
    ushort wLength = content.size();
    static const ushort table[] = { 0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 0xC601, 0x06C0, 0x0780, 0xC741,
            0x0500, 0xC5C1, 0xC481, 0x0440, 0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40, 0x0A00, 0xCAC1, 0xCB81, 0x0B40,
            0xC901, 0x09C0, 0x0880, 0xC841, 0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40, 0x1E00, 0xDEC1, 0xDF81, 0x1F40,
            0xDD01, 0x1DC0, 0x1C80, 0xDC41, 0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641, 0xD201, 0x12C0, 0x1380, 0xD341,
            0x1100, 0xD1C1, 0xD081, 0x1040, 0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240, 0x3600, 0xF6C1, 0xF781, 0x3740,
            0xF501, 0x35C0, 0x3480, 0xF441, 0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41, 0xFA01, 0x3AC0, 0x3B80, 0xFB41,
            0x3900, 0xF9C1, 0xF881, 0x3840, 0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41, 0xEE01, 0x2EC0, 0x2F80, 0xEF41,
            0x2D00, 0xEDC1, 0xEC81, 0x2C40, 0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640, 0x2200, 0xE2C1, 0xE381, 0x2340,
            0xE101, 0x21C0, 0x2080, 0xE041, 0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240, 0x6600, 0xA6C1, 0xA781, 0x6740,
            0xA501, 0x65C0, 0x6480, 0xA441, 0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 0xAA01, 0x6AC0, 0x6B80, 0xAB41,
            0x6900, 0xA9C1, 0xA881, 0x6840, 0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41, 0xBE01, 0x7EC0, 0x7F80, 0xBF41,
            0x7D00, 0xBDC1, 0xBC81, 0x7C40, 0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640, 0x7200, 0xB2C1, 0xB381, 0x7340,
            0xB101, 0x71C0, 0x7080, 0xB041, 0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241, 0x9601, 0x56C0, 0x5780, 0x9741,
            0x5500, 0x95C1, 0x9481, 0x5440, 0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40, 0x5A00, 0x9AC1, 0x9B81, 0x5B40,
            0x9901, 0x59C0, 0x5880, 0x9841, 0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40, 0x4E00, 0x8EC1, 0x8F81, 0x4F40,
            0x8D01, 0x4DC0, 0x4C80, 0x8C41, 0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641, 0x8201, 0x42C0, 0x4380, 0x8341,
            0x4100, 0x81C1, 0x8081, 0x4040, };

    ushort crc = 0x0000;
    while (wLength--) {
        crc = (crc >> 8) ^ table[(crc ^ *nData) & 0xff];
        nData++;
    }
    return crc;
}

void FaceManager::dealRcodeData(const QByteArray &datas)
{
    QByteArrayList contentList = datas.split(',');
    bool ok;
    QDateTime origin_time = QDateTime::fromString("1970-01-01 08:00:00","yyyy-MM-dd hh:mm:ss");
    QDateTime current_time = QDateTime::currentDateTime();//显示时间，格式为：年-月-日 时：分：秒 周几
    qint64 nowTime = origin_time.secsTo(current_time);
    int starTime = contentList.mid(3, 4).join().toInt(&ok, 16);
    int endTime = contentList.mid(7, 4).join().toInt(&ok, 16) * 60 + starTime;
    ushort crc = contentList.mid(20, 2).join().toUShort(&ok, 16);
    ushort crc16 = CRC16_CCITT(datas);
    int type = contentList.at(0).toInt(&ok, 16);
    if(0xF1 == type || 0xF2 == type)
    {
        if (crc == crc16 && nowTime < endTime && nowTime > starTime)
        {
            int userId = contentList.mid(12, 4).join().toInt(&ok, 16);
            QVariantList users = sqlDatabase->sqlSelect(userId);
            if(users.size() > 0)
            {
                QString name = users.value(1).toString();
                hardware->checkOpenDoor();
                qt_debug() << "QRCode PASS !";
            }
            else {

            }
        }
        else
        {

        }
    }
    contentList.clear();
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
    unsigned char *rcodeImage = new unsigned char[VIDEO_WIDTH*VIDEO_HEIGHT* 3 / 2];
    while(true)
    {
        int rcode = switchCtl->m_rcode;
        if(dataShare->m_sync)
        {
            status = true;
            hardware->ctlWDG();
            emit faceTb(tr("正在同步中..."));
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
        if(m_bgrVideoFrame == nullptr) {
//            qt_debug() << ("bgrVideoFrame error\n");
            sleep(4);
            continue;
        }
        if (ir) {
            m_irVideoFrame = nullptr;
            IF_GetIRData(&m_irVideoFrame, VIDEO_WIDTH, VIDEO_HEIGHT);
            if (m_irVideoFrame == nullptr) {
//                qt_debug() << ("irVideoFrame error\n");
                sleep(4);
                continue;
            }
        }
        if(rcode)
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
                dealRcodeData(datas);
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
                m_trackId.append(trackId);
            }
            sort(bgrHandle, bgrLength);
        }
        if (bgrLength > 0)
        {
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
                dataShare->m_offlineFlag = false;
                m_interFace->m_iFaceHandle = bgrHandle;
                m_interFace->m_faceHandle = m_sMFaceHandle;
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
            if(backLightCount > rcode ? 70 : 100)
            {
                dataShare->m_offlineFlag = true;
                backLightCount = 0;
                onBreathingLight();
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
                region = rect.left > 92 && rect.top > 281 && rect.right < 708 && rect.bottom < 861;
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
            buf.track_id = m_trackId[i];
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
    datas << QDateTime::currentDateTime().addSecs(28800).toString("yyyy-MM-dd HH:mm:ss") << "" << "1" << "" << "0" << "";
    sqlDatabase->sqlInsertOffline(0, id, 1, 0, 0, datas);
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
                sqlDatabase->sqlDeleteFail(id);
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
            insertFaceGroup(sqlDatabase->m_groupHandle, ret.data(), 512, id);
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
