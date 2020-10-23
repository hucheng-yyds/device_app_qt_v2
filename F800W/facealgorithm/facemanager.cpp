#include "facemanager.h"

QSemaphore g_usedSpace(0);

FaceManager::FaceManager()
{
    m_isIdentify = false;
    m_timer = new CountDown;
}

void FaceManager::setFaceInter(FaceInterface *inter)
{
    m_interFace = inter;
}

void FaceManager::run()
{
    FaceRect rect;
    int saveLeft[5];
    int saveTop[5];
    int saveRight[5];
    int saveBottom[5];
    int offset = 10;
    while(true)
    {
        m_bgrVideoFrame = nullptr;
        IF_GetData(&m_bgrVideoFrame, VIDEO_WIDTH, VIDEO_HEIGHT);
        if(m_bgrVideoFrame == nullptr) {
//            qt_debug() << ("bgrVideoFrame error\n");
            sleep(4);
            continue;
        }
        if (1) {
            m_irVideoFrame = nullptr;
            IF_GetIRData(&m_irVideoFrame, VIDEO_WIDTH, VIDEO_HEIGHT);
            if (m_irVideoFrame == nullptr) {
//                qt_debug() << ("irVideoFrame error\n");
                sleep(4);
                continue;
            }
        }
        FaceHandle *bgrHandle;
        int bgrLength;
        m_mutex.lock();
        detect((const char *)m_bgrVideoFrame->stVFrame.u64VirAddr[0], m_bgrVideoFrame->stVFrame.u32Stride[0],
                m_bgrVideoFrame->stVFrame.u32Height, NV21, 0.75, &bgrHandle, &bgrLength);
        m_mutex.unlock();
        m_trackId.clear();
        m_trackId.resize(bgrLength);
        track(bgrHandle, bgrLength, m_trackId.data());
        int ret = sort(bgrHandle, bgrLength);
        if (bgrLength > 0) {
            hardware->ctlIrWhite(IR_WHITE);
            for(int i = 0; i < m_sMFaceHandle.size(); i++) {
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
            if (!m_interFace->m_iFaceHandle && m_isIdentify) {
                m_interFace->m_iFaceHandle = bgrHandle;
                m_interFace->m_faceHandle = m_sMFaceHandle;
                m_interFace->m_count = bgrLength;
                m_interFace->m_iStop = false;
                memcpy(m_interFace->m_bgrImage, (const char *)m_bgrVideoFrame->stVFrame.u64VirAddr[0], VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2);
                if (1) {
                    memcpy(m_interFace->m_irImage, (const char *)m_irVideoFrame->stVFrame.u64VirAddr[0], VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2);
                }
                g_usedSpace.release();
            } else {
                releaseAllFace(bgrHandle, bgrLength);
            }
        } else {
            m_interFace->m_iStop = true;
            emit hideFaceFocuse();
//            if(settings->isTemp)
//            {
//                emit stopTemp();
//            }
//            emit hideRect(false);
//            if(m_lightStatus)
//            {
//                m_lightCount++;
//                if(m_lightCount > 100)
//                {
//                    m_lightCount = 0;
//                    hardware->setBLNOn();
//                }
//            }
//            else {
//                m_lightCount = 0;
//            }
            releaseAllFace(bgrHandle, bgrLength);
        }
        hardware->checkOpenDoor();
        hardware->ctlWDG();
        if (1) {
            IF_ReleaseIRData(m_irVideoFrame);
        }
        IF_ReleaseData(m_bgrVideoFrame);
    }
}

bool FaceManager::filter(const FaceRect &rect)
{
    bool pass = true;
    int width = rect.right - rect.left;
    if (0)
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
            if (0)
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

int FaceManager::sort(FaceHandle *faceHandle, int count)
{
    int i, j;
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
            if (!m_timer->expired())
            {
                if (m_interFace->m_quality && !m_interFace->m_iStop)
                {
                    m_isIdentify = false;
                }
            }
            else
            {
                m_timer->countdown_ms(3*1000);
            }
        }
        else
        {
            m_timer->countdown_ms(3*1000);
        }
    }
    return 1;
}

bool FaceManager::init()
{
    FaceModels models;
    memset(&models, 0 , sizeof(FaceModels));
    models.group_model_path = "model/3516_search_A_encrypt.bin";
    models.detect_model = "model/3516_detect_F_encrypt.bin";
    models.postfilter_model = "model/3516_postfilter_F_encrypt.bin";
    models.refine_model = "model/3516_landmark_A_encrypt.bin";
    models.anchor_path = "model/3516_anchor_A_encrypt.bin";
    models.pose_blur_model = "model/3516_poseblur_A_encrypt.bin";
    models.liveness_bgr_model = "model/3516_live_rgb_E_encrypt.bin";
    models.liveness_bgrir_model = "model/3516_live_rgbir_E_encrypt.bin";
    models.liveness_ir_model = "";
    models.age_gender_model = "model/3516_age_gender_A_encrypt.bin";
    models.rc_model = "model/3516_rc_attr_A_encrypt.bin";
    models.occl_model = "model/3516_occlusion_A_encrypt.bin";
    models.stn_model = "model/3516_preFeat_Manta_encrypt.bin";
    models.feature_model = "model/3516_feat_Manta_encrypt.bin";

    setLogLevel(LOG_LEVEL_ERROR);
    int ret = ::init(models);
    if (ret != RET_OK) {
//        qt_debug() << "init failed !";
        return false;
    }
//    qt_debug() << "Start !";
    set_detect_config(0.3, 0.5);
    set_match_config(0.99, -35.61, 0.99, 4.26, 0.4);
    qDebug() << "================================";
    createFaceGroup(&m_interFace->m_groupHandle);
    return true;
}
