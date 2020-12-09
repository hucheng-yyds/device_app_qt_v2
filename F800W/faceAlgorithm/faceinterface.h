#ifndef FACEINTERFACE_H
#define FACEINTERFACE_H
#include <QSemaphore>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "hi_comm_video.h"
#include "switch.h"
#include "qalhardware.h"
#include "sqldatabase.h"

#define VIDEO_WIDTH     800
#define VIDEO_HEIGHT    1280

struct FaceTrack{
    int track_id;
    int index;
};

// 算法人脸检测和人脸比对共用api类
class FaceInterface
{
public:
    explicit FaceInterface();
    FaceHandle *m_iFaceHandle;
    QVector<FaceTrack> m_faceHandle;
    int m_count;
    bool m_iStop;
    bool m_quality;
    bool m_localFaceSync;
    unsigned char *m_irImage;
    unsigned char *m_bgrImage;
    QMutex m_mutex;
};

#endif // FACEINTERFACE_H
