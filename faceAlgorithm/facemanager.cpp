#include "facemanager.h"

#ifdef __cplusplus
//#if __cplusplus
extern "C" {
//#endif /* __cplusplus */
#endif  /* __cplusplus */
extern int AW_MPI_ISP_SetLocalExposureArea(ISP_DEV IspDev, SIZE_S Res, RECT_S RoiRgn);
#ifdef __cplusplus
//#if __cplusplus
}
//#endif /* __cplusplus */
#endif  /* __cplusplus */

FaceManager::FaceManager()
{
    m_isIdentify = false;
}

FaceManager::~FaceManager()
{
    DS_ReleaseAppCall(m_ptrAppData);
    qt_debug() << "DS_ReleaseAppCall";
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
//    hardware->ctlLed(OFF);
//    hardware->ctlBLN(ON);
    hardware->ctlIrWhite(OFF);

}

void FaceManager::run()
{
    while (1) {
        m_bgrVideoFrame = nullptr;
        IF_GetData(&m_bgrVideoFrame, VIDEO_WIDTH, VIDEO_HEIGHT);
        if(m_bgrVideoFrame == nullptr) {
            qt_debug() << ("bgrVideoFrame error\n");
            sleep(4);
            continue;
        }
        if (switchCtl->m_ir) {
            m_irVideoFrame = nullptr;
            IF_GetIRData(&m_irVideoFrame, SOURCE_WIDTH, SOURCE_HEIGHT);
            if (m_irVideoFrame == nullptr) {
                qt_debug() << ("irVideoFrame error\n");
                sleep(4);
                continue;
            }
        }
        if(dataShare->m_sync) {
            status = true;
//            hardware->ctlWDG();
            emit faceTb(tr("正在同步中..."));
            hardware->ctlWhite(OFF);
            if(!dataShare->m_netStatus)
            {
                dataShare->m_sync = false;
            }
            qt_debug() << "dataShare->m_sync:" << dataShare->m_sync;
            hardware->checkCloseDoor();
            msleep(500);
            continue;
        } else {
            m_ptrAppData->ptrFaceIDInData->FuncFlag = CTRL_RECOGNITION;
        }
        if(status)
        {
            status = false;
            emit faceTb("");
        }
        m_ptrAppData->ptrFaceIDInData->ptrYUVSubStream[0] = (uchar*)m_bgrVideoFrame->VFrame.mpVirAddr[0];
        m_ptrAppData->ptrFaceIDInData->ptrYUVSubStream[1] = (uchar*)m_bgrVideoFrame->VFrame.mpVirAddr[1];
        if (switchCtl->m_ir)
        {
            m_ptrAppData->ptrFaceIDInData->ptrIRSubStream = (uchar*)m_irVideoFrame->VFrame.mpVirAddr[0];
        }
        m_ptrAppData->ptrFaceIDInData->colorVideoType = NV12;
        m_ptrAppData->ptrFaceIDInData->irVideoType = GRAY;
        m_ptrAppData->ptrFaceIDInData->ptrBGRImage = nullptr;
        m_ptrAppData->ptrFaceIDInData->bgrImageWidth = 0;
        m_ptrAppData->ptrFaceIDInData->bgrImageHeight = 0;
        DS_SetGetAppCall(m_ptrAppData);
//        qt_debug() << "Get People Num " << m_ptrAppData->ptrFaceIDOutData->curFaceNum << m_ptrAppData->ptrFaceIDOutData->curStatus;
        if (m_ptrAppData->ptrFaceIDOutData->curFaceNum > 0) {
            sort();
            m_ptrAppData->ptrFaceIDOutData->curFaceNum = 1;
            for (int i = 0; i < m_ptrAppData->ptrFaceIDOutData->curFaceNum; ++i)
            {
                DS_FaceInfo &ptrFaceInfo = m_ptrAppData->ptrFaceIDOutData->faceInfo[i];
//                qt_debug() << "ptrFaceInfo.trackID:" << ptrFaceInfo.trackID;
//                qt_debug() << "totalRegPersonsNum" << m_ptrAppData->ptrFaceIDOutData->totalRegPersonsNum;

//                int offset = 10;
//                offset = (ptrFaceInfo.XMax - ptrFaceInfo.XMin) / 20;
//                if (((qAbs(saveLeft[i] - ptrFaceInfo.XMin) > offset) ||
//                     (qAbs(saveTop[i] - ptrFaceInfo.YMin) > offset)) &&
//                        ((qAbs(saveRight[i] - ptrFaceInfo.XMax) > offset) ||
//                         (qAbs(saveBottom[i] - ptrFaceInfo.YMax) > offset))) {
//                    saveLeft[i] = ptrFaceInfo.XMin;
//                    saveTop[i] = ptrFaceInfo.YMin;
//                    saveRight[i] = ptrFaceInfo.XMax;
//                    saveBottom[i] = ptrFaceInfo.YMax;
//                }
                hardware->ctlIrWhite(ON);
                emit showFaceFocuse(saveLeft[i] * 1.66, saveTop[i] * 1.6, saveRight[i] * 1.66, saveBottom[i] * 1.6, i, ptrFaceInfo.trackID);
//                qt_debug() << qAbs(saveLeft[i] - ptrFaceInfo.XMin) << offset;
                /*动态曝光测试接口*/
//                SIZE_S awSize = {ptrFaceInfo.XMax - ptrFaceInfo.XMin, ptrFaceInfo.YMax - ptrFaceInfo.YMin};
//                RECT_S awRect = {ptrFaceInfo.XMin, ptrFaceInfo.YMin, awSize.Width, awSize.Height};
//                AW_MPI_ISP_SetLocalExposureArea(0 ,awSize ,awRect);
                if (m_interFace->m_iStop) {
                    m_interFace->m_iStop = false;
                    m_interFace->m_faceHandle << ptrFaceInfo;
//                    qt_debug() << "m_ptrAppData->ptrFaceIDOutData->returnImg:"
//                               << m_ptrAppData->ptrFaceIDOutData->returnImgW
//                               << m_ptrAppData->ptrFaceIDOutData->returnImgH;
                    if (m_ptrAppData->ptrFaceIDOutData->returnImgW && m_ptrAppData->ptrFaceIDOutData->returnImgH) {
                        memcpy(m_interFace->m_bgrImage, m_ptrAppData->ptrFaceIDOutData->returnRegsucImg,
                               m_ptrAppData->ptrFaceIDOutData->returnImgW * m_ptrAppData->ptrFaceIDOutData->returnImgH * 3);
                        m_ptrAppData->ptrFaceIDOutData->returnImgW = 0;
                        m_ptrAppData->ptrFaceIDOutData->returnImgH = 0;
                    }
                    m_interFace->m_usedSpace.release();
                }
            }
            if (m_ptrAppData->ptrFaceIDOutData->curStatus == RETURN_SAVE_REGINFO_SUCCESS ||
                    m_ptrAppData->ptrFaceIDOutData->curStatus == RETURN_SAVE_HAVE_REGISTERED)
            {
                m_ptrAppData->ptrFaceIDInData->FuncFlag = CTRL_RECOGNITION;
            }

            if (m_ptrAppData->ptrFaceIDOutData->curStatus == RETURN_UPDATE_REGFEATURE_SUCCESS) m_ptrAppData->ptrFaceIDInData->FuncFlag = CTRL_RECOGNITION;
            if (m_ptrAppData->ptrFaceIDOutData->curStatus == RETURN_CANCEL_SUCCESS) m_ptrAppData->ptrFaceIDInData->FuncFlag = CTRL_RECOGNITION;
            if (m_ptrAppData->ptrFaceIDOutData->curStatus == RETURN_DELETE_SUCCESS) m_ptrAppData->ptrFaceIDInData->FuncFlag = CTRL_RECOGNITION;
            if (m_ptrAppData->ptrFaceIDOutData->curStatus == RETURN_CLEAR_RECOGNITION_SUCCESS) m_ptrAppData->ptrFaceIDInData->FuncFlag = CTRL_RECOGNITION;
        } else {
//            m_interFace->m_iStop = true;
            emit hideFaceFocuse();
            backLightCount++;
            if(backLightCount > 100)
            {
                dataShare->m_offlineFlag = true;
                backLightCount = 0;
                onBreathingLight();
            }
        }
        hardware->checkCloseDoor();
//        hardware->ctlWDG();
        if (switchCtl->m_ir)
        {
            IF_ReleaseIRData(m_irVideoFrame);
        }
        IF_ReleaseData(m_bgrVideoFrame);
#if 0
        static Countdown timer;
        static int j = 0;
        static int m_count = 0;
        if (j > 100) {
            double d_ms = m_count / 100.00;
            qt_debug() << "m_count:" << m_count << d_ms << timer.right_ms();
            j = 0;
            m_count = 0;
            Countdown t(100);
            {
                QFile file("YUV_BGR_DATA.yuv");
                qt_debug() << file.open(QIODevice::WriteOnly);
                memcpy(m_interFace->m_bgrImage, m_bgrVideoFrame->VFrame.mpVirAddr[0], VIDEO_WIDTH * VIDEO_HEIGHT);
                memcpy(m_interFace->m_bgrImage + VIDEO_WIDTH * VIDEO_HEIGHT, m_bgrVideoFrame->VFrame.mpVirAddr[1], VIDEO_WIDTH * VIDEO_HEIGHT / 2);
                file.write((const char *)m_interFace->m_bgrImage, VIDEO_WIDTH * VIDEO_HEIGHT * 3 / 2);
                file.close();
            }
            QFile file("YUV_IR_DATA.yuv");
            qt_debug() << file.open(QIODevice::WriteOnly);
            memcpy(m_interFace->m_irImage, m_irVideoFrame->VFrame.mpVirAddr[0], SOURCE_WIDTH * SOURCE_HEIGHT);
            memcpy(m_interFace->m_irImage + SOURCE_WIDTH * SOURCE_HEIGHT, m_irVideoFrame->VFrame.mpVirAddr[1], SOURCE_WIDTH * SOURCE_HEIGHT / 2);
            file.write((const char *)m_interFace->m_irImage, SOURCE_WIDTH * SOURCE_HEIGHT * 3 / 2);
            file.close();
            qt_debug() << "memcpy:" << t.right_ms();
        }
        j ++;
        m_count += timer.right_ms();
        timer.countdown_ms(300);
#endif
    }
}

bool FaceManager::filter(const QRect &rect)
{
    bool pass = true;
    int width = rect.width();
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
                region = rect.left() > 92 && rect.top() > 281 && rect.right() < 708 && rect.bottom() < 861;
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

void FaceManager::sort()
{
    for (int i=0; i<m_ptrAppData->ptrFaceIDOutData->curFaceNum-1; ++i)  //比较n-1轮
    {
        for (int j=0; j<m_ptrAppData->ptrFaceIDOutData->curFaceNum-1-i; ++j)  //每轮比较n-1-i次,
        {
            DS_FaceInfo &info0 = m_ptrAppData->ptrFaceIDOutData->faceInfo[j];
            DS_FaceInfo &info1 = m_ptrAppData->ptrFaceIDOutData->faceInfo[j+1];
            int width0 = info0.XMax - info0.XMin;
            int width1 = info1.XMax - info1.XMin;
            if (width0 < width1)
            {
                DS_FaceInfo buf;
                buf = info0;
                info0 = info1;
                info1 = buf;
            }
        }
    }
}

void FaceManager::ctlOpenDoor(int id)
{
//    hardware->ctlLed(GREEN);
    hardware->checkOpenDoor();
    QStringList datas;
    datas.clear();
    datas << QDateTime::currentDateTime().addSecs(28800).toString("yyyy-MM-dd HH:mm:ss") << "" << "1" << "" << "0" << "" << "" << "" << "" << "" << "";
    sqlDatabase->sqlInsertOffline(0, id, 1, 0, 0, 0, datas);
}

void FaceManager::insertFaceGroups(int id, const QString &username, const QString &time, const QString &photoname, const QString &iphone)
{
//    int count;
//    return ;
    QString file = QString::number(id) + ".jpg";
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
        if (nullptr == m_ptrAppData) {
            return ;
        }
        int tmpW = image.cols;
        int tmpH = image.rows;
        AppCall *ptrAppData = m_ptrAppData;
        ptrAppData->ptrFaceIDInData->FuncFlag = CTRL_REGISTER_BY_PIC;
        ptrAppData->ptrFaceIDInData->ptrBGRImage = image.data;
        ptrAppData->ptrFaceIDInData->bgrImageWidth = tmpW;
        ptrAppData->ptrFaceIDInData->bgrImageHeight = tmpH;
        ptrAppData->ptrFaceIDInData->importFaceID = id;

        DS_SetGetAppCall(m_ptrAppData);

        //处理注册结果
        if (ptrAppData->ptrFaceIDOutData->curStatus == RETURN_REGISTER_SUCCESS)
        {
            ptrAppData->ptrFaceIDInData->FuncFlag = CTRL_SAVE_REGINFO;
            int saveTime = 0;
            //相当于阻塞
            while (ptrAppData->ptrFaceIDOutData->curStatus != RETURN_SAVE_REGINFO_SUCCESS && saveTime < 100)
            {
                saveTime += 1;
                DS_SetGetAppCall(m_ptrAppData);
            }
        }
        else
        {
//            if (0 == count)
//            {
//                sqlDatabase->sqlInsertFail(id,  2);
//                qt_debug() << "faceHandle is null !" << id << "2";
//            }
//            else
//            {
//                qt_debug() << "failInsert" << 3;
//                sqlDatabase->sqlInsertFail(id, 3);
//            }
            printf("===Pic Reg out_Flag:%x(REGISTERing)===\n", ptrAppData->ptrFaceIDOutData->curStatus);
        }

        //get outPutData
        DS_FaceIDOutData* tmp = ptrAppData->ptrFaceIDOutData;
        if (tmp->curStatus != RETURN_SAVE_REGINFO_SUCCESS)
        {
            printf("pic reg fail\n");
            ptrAppData->ptrFaceIDInData->FuncFlag = CTRL_REGISTER_CANCEL;
            DS_SetGetAppCall(m_ptrAppData);
        }
        else
        {
            tmp->regProgress = 100.0f;
            ptrAppData->ptrFaceIDInData->ptrBGRImage = nullptr;
            qt_debug() << ptrAppData->ptrFaceIDOutData->faceID;
            sqlDatabase->sqlInsert(id, username, time, "", photoname, iphone);
            sqlDatabase->sqlDeleteFail(id);
        }
    }
    QFile::remove(file);
}

void FaceManager::removeFaceGroup(int id)
{
    qt_debug() << id;
    if (m_ptrAppData->ptrFaceIDOutData->totalRegPersonsNum > 0) {
        m_ptrAppData->ptrFaceIDInData->FuncFlag = CTRL_DELETE_ONE;
        m_ptrAppData->ptrFaceIDInData->importFaceID = id;
        DS_SetGetAppCall(m_ptrAppData);
        qt_debug() << m_ptrAppData->ptrFaceIDOutData->curStatus;
    }

//    if (m_ptrAppData->ptrFaceIDOutData->curStatus == RETURN_DELETE_SUCCESS) {
//        sqlDatabase->sqlDelete(id);
//    }
}

AppCall *FaceManager::DS_CreateAppCall(const char *ptrRegFilePath, const char *ptrModelFileAbsDir, const char *ptrFaceImgFilePath)
{
    AppCall *ptrAppData = (AppCall *)malloc(sizeof(AppCall));

    if(!ptrAppData)
    {
        qDebug("ptrAppData init NULL\n");
        return NULL;
    }

    qDebug("*****RegFilePath:%s  ModelFileAbsDir:%s  FaceImgFilePath:%s *******\n", ptrRegFilePath, ptrModelFileAbsDir, ptrFaceImgFilePath);

    ptrAppData->ptrFaceIDParas = (DS_FaceIDParas *)malloc(sizeof(DS_FaceIDParas));
    ptrAppData->ptrFaceIDInData = (DS_FaceIDInData *)malloc(sizeof(DS_FaceIDInData));
    ptrAppData->ptrFaceIDOutData = (DS_FaceIDOutData *)malloc(sizeof(DS_FaceIDOutData));
    if (!ptrAppData->ptrFaceIDParas || !ptrAppData->ptrFaceIDInData || !ptrAppData->ptrFaceIDOutData)
    {
        qDebug("AppData param NULL\n");
//        DS_ReleaseAppCall(ptrAppData);
        return NULL;
    }

    memset(ptrAppData->ptrFaceIDInData, 0, sizeof(DS_FaceIDInData));
    memset(ptrAppData->ptrFaceIDOutData, 0, sizeof(DS_FaceIDOutData));

    //初始化结构体
    DS_FaceIDParas *ptrFaceIDParas = ptrAppData->ptrFaceIDParas;

    //初始化图像宽高
    ptrFaceIDParas->imgWidth[0] = VIDEO_WIDTH;
    ptrFaceIDParas->imgHeight[0] = VIDEO_HEIGHT;
    ptrFaceIDParas->imgWidth[1] = SOURCE_WIDTH;
    ptrFaceIDParas->imgHeight[1] = SOURCE_HEIGHT;
    ptrFaceIDParas->irMode = IR_DISABLE;
    ptrFaceIDParas->reRegsucessMode = RETURN_ENABLE;

    memcpy(ptrFaceIDParas->ptrRegFilePath, ptrRegFilePath, strlen(ptrRegFilePath)+1);
    memcpy(ptrFaceIDParas->ptrModelFileAbsDir, ptrModelFileAbsDir, strlen(ptrModelFileAbsDir) + 1);
    memcpy(ptrFaceIDParas->ptrFaceImgFilePath, ptrFaceImgFilePath, strlen(ptrFaceImgFilePath) + 1);

    ptrAppData->ptrFaceHandle = DS_CreateFaceIDContext(ptrAppData->ptrFaceIDParas);
    if(!ptrAppData->ptrFaceHandle)
    {
        qDebug("FaceIDContext init Fail\n");
        DS_ReleaseAppCall(ptrAppData);
        return NULL;
    }

    const char *ptrVersionID = DS_GetFaceIDVersion(ptrAppData->ptrFaceHandle);
    qDebug("GetCurrentVersionID %s\n", ptrVersionID);
//    DS_SetPrintLevel(ptrAppData->ptrFaceHandle, SET_LOG_DEBUG);

//    struct sched_param detect_param, identify_param;

//    pthread_attr_init(&detect_attr);
//    detect_param.sched_priority = 1; //越大优先级越高
//    pthread_attr_setinheritsched(&detect_attr, PTHREAD_EXPLICIT_SCHED);
//    pthread_attr_setschedpolicy(&detect_attr, SCHED_RR);
//    pthread_attr_setschedparam(&detect_attr, &detect_param);

//    pthread_attr_init(&identify_attr);
//    identify_param.sched_priority = 2; //越大优先级越高
//    pthread_attr_setinheritsched(&identify_attr, PTHREAD_EXPLICIT_SCHED);
//    pthread_attr_setschedpolicy(&identify_attr, SCHED_RR);
//    pthread_attr_setschedparam(&identify_attr, &identify_param);

//    int ret = 0;
//    ret = pthread_create(&detect_pid, &detect_attr, DS_FaceDetition, ptrAppData->ptrFaceHandle);
//    if (ret != 0)
//    {
//        printf("DS_FaceDetition thread error\n");
//    }
//    ret = pthread_create(&identify_pid, &identify_attr, DS_FaceRecognition, ptrAppData->ptrFaceHandle);
//    if (ret != 0)
//    {
//        printf("DS_FaceRecognition thread error\n");
//    }
//    qt_debug() << detect_pid << "detect_pid";

    return ptrAppData;
}

int FaceManager::DS_ReleaseAppCall(AppCall *ptrAppData)
{
    if (!ptrAppData)
    {
        qDebug("ptrAppData NULL\n");
        return 0;
    }

    int ret = DS_ReleaseFaceIDContext(ptrAppData->ptrFaceHandle);

    if (ptrAppData->ptrFaceIDParas)
    {
        free(ptrAppData->ptrFaceIDParas);
        ptrAppData->ptrFaceIDParas = NULL;
    }
    if (ptrAppData->ptrFaceIDInData)
    {
        free(ptrAppData->ptrFaceIDInData);
        ptrAppData->ptrFaceIDInData = NULL;
    }
    if (ptrAppData->ptrFaceIDOutData)
    {
        ptrAppData->ptrFaceIDOutData->faceInfo.clear();
        free(ptrAppData->ptrFaceIDOutData);
        ptrAppData->ptrFaceIDOutData = NULL;
    }

    pthread_join(detect_pid, NULL);
    pthread_attr_destroy(&detect_attr);
    pthread_join(identify_pid, NULL);
    pthread_attr_destroy(&identify_attr);

    free(ptrAppData);
    ptrAppData = NULL;

    return ret;
}

int FaceManager::DS_SetGetAppCall(AppCall *ptrAppData)
{
    if (!ptrAppData)
    {
        qDebug("ptrAppData NULL\n");
        return 0;
    }

    int reVal = DS_FaceIDProcess(ptrAppData->ptrFaceHandle, ptrAppData->ptrFaceIDInData, ptrAppData->ptrFaceIDOutData);

    return reVal;
}

bool FaceManager::init()
{
    qt_debug() << system("./setGamma.sh") << "./setGamma.sh";;
    QDir dir(dataShare->m_offlinePath);
    if (!dir.exists()) {
        qt_debug() << dir.mkpath(dataShare->m_offlinePath);
    }
    dir.setPath(dataShare->m_regInfoPath);
    if (!dir.exists()) {
        qt_debug() << dir.mkpath(dataShare->m_regInfoPath);
    }
    QByteArray ptrRegFilePath = dataShare->m_regInfoPath.toUtf8();
    QByteArray ptrFaceImgFilePath = dataShare->m_regInfoPath.toUtf8();
    QByteArray ptrModelFileAbsDir = "./models/";
    m_ptrAppData = DS_CreateAppCall(ptrRegFilePath, ptrModelFileAbsDir, ptrFaceImgFilePath);
    if (!m_ptrAppData) {
        return false;
    }
    m_ptrAppData->ptrFaceIDInData->FuncFlag = CTRL_RECOGNITION;
    m_ptrAppData->ptrFaceIDInData->faceThresh[0] = -0.83;
    m_ptrAppData->ptrFaceIDInData->faceThresh[1] = -0.83;
    m_ptrAppData->ptrFaceIDInData->faceThresh[2] = -0.75;
    m_ptrAppData->ptrFaceIDInData->faceThresh[3] = -0.81;
    m_ptrAppData->ptrFaceIDInData->recFaceMode = REC_MAX_FACE;
    m_ptrAppData->ptrFaceIDInData->minRecFaceW = 0;
    m_ptrAppData->ptrFaceIDInData->maxRecFaceW = 0;
    sqlDatabase->sqlSelectAllUserId();
    return true;
}

//void FaceManager::localFaceInsert()
//{
//    QSet<int> ids = sqlDatabase->sqlSelectAllUserId();
//    foreach (int id, ids)
//    {
//        emit faceTb(tr("加载人员中..."));
//        QString value = sqlDatabase->sqlSelectAllUserFeature(id);
//        if ("0" != value && !value.isEmpty())
//        {
//            QStringList result = value.split(",");
//            QVector<char> ret;
//            int size = result.size();
//            ret.resize(size);
//            for (int i = 0; i < size; i ++)
//            {
//                ret[i] = result.at(i).toFloat();
//            }
//            insertFaceGroup(sqlDatabase->m_groupHandle, ret.data(), 512, id);
//        }
//        else {
//            qt_debug() << "value 0" << value;
//        }
//    }
//    emit faceTb(tr(""));
//    updateIdentifyValue();
//}
