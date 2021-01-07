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
    ~FaceManager();
    // 初始化人脸识别算法
    bool init();
    // 人脸检测和比对共享资源接口
    void setFaceInter(FaceInterface *inter);

protected:
    virtual void run();

public slots:
    // 开门
    void ctlOpenDoor(int id);
    // 人员图片入库
    void insertFaceGroups(int id, const QString &username, const QString &time, const QString &photoname, const QString &iphone);
    void removeFaceGroup(int id);
    void clearFaceGroup();

private:
    void saveImageIr(const QString &path);
    void saveImageBgr(const QString &path);
    void autoIRExposure(uchar *bgrYData);
    void autoBGRExposure();
    AppCall* DS_CreateAppCall(const char* ptrRegFilePath, const char* ptrModelFileAbsDir, const char* ptrFaceImgFilePath);
    int DS_ReleaseAppCall(AppCall *ptrAppData);
    int DS_SetGetAppCall(AppCall *ptrAppData);
    // 根据人脸框的大小过滤每帧图片的人脸
    bool filter(const QRect &rect);
    // 取出每帧图片的最大人脸，并判断下轮比对是否开始
    void sort();
    // 待机状态下打开蓝色呼吸灯
    void onBreathingLight();
    void saveImage(int x, int y, int w, int h);
    // 本地人脸数据入库
    void localFaceInsert();
    // 根据人脸底库更新阈值
    void updateIdentifyValue();
    // 是否已到定时的时长，是返回true，否返回false
    bool faceExpired();
    // 设置定时时长。 从当前时间开始的定时时长，单位ms
    void faceCountdown_ms(int ms);
    // 是否已到定时的时长，是返回true，否返回false
    bool expired();
    // 设置定时时长。 从当前时间开始的定时时长，单位ms
    void countdown_ms(int ms);

signals:
    // 显示人脸框 ui界面显示
    void showFaceFocuse(int left, int top, int right, int bottom, int index, int trackId);
    // 检测人脸结果显示
    void showFaceResult();
    // 隐藏人脸框 控制ui界面消失人脸框
    void hideFaceFocuse();
    // 人员同步和升级状态显示
    void faceTb(const QString &text);
    void showIr();

private:
    VIDEO_FRAME_INFO_S *m_bgrVideoFrame;
    VIDEO_FRAME_INFO_S *m_irVideoFrame;
    QVector<int> m_trackId;
    FaceInterface *m_interFace;
    AppCall *m_ptrAppData;
    pthread_t detect_pid, identify_pid;
    pthread_attr_t detect_attr, identify_attr;
    bool m_isIdentify;
    QMutex m_mutex;
    int saveLeft[5];
    int saveTop[5];
    int saveRight[5];
    int saveBottom[5];
    int backLightCount = 0;
    qint64 m_endTimerMs;
    qint64 m_checkTimerMs;
    bool status = false;

};
#endif // FACEMANAGER_H
