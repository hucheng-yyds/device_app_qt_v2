#ifndef FACEINTERFACE_H
#define FACEINTERFACE_H
#include <QSemaphore>
#include <QRect>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include "switch.h"
#include "qalhardware.h"
#include "sqldatabase.h"
#include "Countdown.h"

#define VIDEO_WIDTH     360
#define VIDEO_HEIGHT    640
#define SOURCE_WIDTH    480
#define SOURCE_HEIGHT   640

typedef struct App_Call
{
    DS_FaceIDParas *ptrFaceIDParas;
    DS_FaceIDInData *ptrFaceIDInData;
    DS_FaceIDOutData *ptrFaceIDOutData;

    void *ptrFaceHandle;
}AppCall;

// 算法人脸检测和人脸比对共用api类
class FaceInterface
{
public:
    explicit FaceInterface();
    AppCall *m_ptrAppData;
    QVector<DS_FaceInfo> m_faceHandle;
    QSemaphore m_usedSpace;
    int m_count;
    bool m_iStop;
    bool m_quality;
    bool m_localFaceSync;
    unsigned char *m_irImage;
    unsigned char *m_bgrImage;
    QMutex m_mutex;
};

#endif // FACEINTERFACE_H
