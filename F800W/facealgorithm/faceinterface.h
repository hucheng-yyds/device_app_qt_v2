#ifndef FACEINTERFACE_H
#define FACEINTERFACE_H
#include <QSemaphore>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "switch.h"
#include "face_sdk.h"
#include "hi_comm_video.h"

#define VIDEO_WIDTH     800
#define VIDEO_HEIGHT    1280

struct MFaceHandle{
    FaceRect rect;
    int track_id;
    int index;
};

// 算法人脸检测和人脸比对共用api类
class FaceInterface
{
public:
    explicit FaceInterface();
    FaceHandle *m_iFaceHandle;
    QVector<MFaceHandle> m_faceHandle;
    int m_count;
    bool m_iStop;
    bool m_quality;
    unsigned char *m_irImage;
    unsigned char *m_bgrImage;
    QMutex m_mutex;
    FaceGroupHandle m_groupHandle;
};

#endif // FACEINTERFACE_H
