#ifndef FACEMANAGER_H
#define FACEMANAGER_H
#include <QThread>
#include <QMutex>
#include "faceinterface.h"

// 人脸检测算法处理
class FaceManager : public QThread
{
    Q_OBJECT
public:
    FaceManager();
    // 初始化人脸识别算法
    bool init();
    // 人脸检测和比对共享资源接口
    void setFaceInter(FaceInterface *inter);
protected:
    virtual void run();

private:
    // 根据人脸框的大小过滤每帧图片的人脸
    bool filter(const FaceRect &rect);
    // 取出每帧图片的最大人脸，并判断下轮比对是否开始
    int sort(FaceHandle *faceHandle, int count);

signals:
    // 显示人脸框
    void showFaceFocuse(int left, int top, int right, int bottom, int index, int trackId);
    // 隐藏人脸框
    void hideFaceFocuse();

private:
    VIDEO_FRAME_INFO_S *m_bgrVideoFrame;
    VIDEO_FRAME_INFO_S *m_irVideoFrame;
    QVector<int> m_trackId;
    QVector<MFaceHandle> m_sMFaceHandle;
    FaceInterface *m_interFace;
    bool m_isIdentify;
    QMutex m_mutex;
    CountDown *m_timer;
};
#endif // FACEMANAGER_H
