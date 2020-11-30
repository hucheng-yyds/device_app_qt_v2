#ifndef FACEIDENTIFY_H
#define FACEIDENTIFY_H
#include <QThread>
#include "faceinterface.h"
#include "tempcallback.h"

class FaceIdentify : public QThread, public TempCallBack
{
    Q_OBJECT
public:
    explicit FaceIdentify();
    // 人脸检测和比对共享资源接口
    void setFaceInter(FaceInterface *inter);

protected:
    virtual void run();
    void setTempResult(const QString &tempVal, int result);

public slots:
    void dealIcData(int mid, const QString &cardNo);
private:
    // 判断早中晚
    void judgeDate();
    // 人证比对
    bool idCardFaceComparison(char *feature_result);
    // 处理开门权限
    QStringList dealOpencondition(int faceId);
    // 是否已到定时的时长，是返回true，否返回false
    bool identifyExpired();
    // 设置定时时长。 从当前时间开始的定时时长，单位ms
    void identifyCountdown_ms(int ms);

signals:
    // 韦根输出
    void wgOut(const QByteArray &number);
    // 口罩和安全帽显示
    void maskHelmet(int flag);
    // ic状态显示
    void icResultShow(int result, const QString &cardNo, const QString &showInfo);
    // 人脸检测结果显示 ui显示
    void faceResultShow(const QString &name, int index, int trackId, const QString &result, const QString &showInfo);
    // 显示身份证信息
    void idCardResultShow(int flag, const QString &name, const QString &result, const QString &showInfo);
    // 开始测温信号
    void startTemp();
    // UI显示正在测温
    void showStartTemp();
    // 显示测温状态和结果
    void tempShow(const QString &tempVal, int result);
    // 实时上传记录
    void uploadopenlog(int id, int userId, const QString &photo, int isOver,int type, int isTemp, int sex, const QStringList &datas);

private:
    FaceInterface *m_interFace;
    unsigned char *m_irImage;
    unsigned char *m_bgrImage;
    QVector<MFaceHandle> m_iMFaceHandle;
    bool m_tempFlag;
    QString m_tempVal;
    QString m_faceInfo;
    int m_tempResult;
    QString m_cardNo;
    bool m_cardWork;
    qint64 m_endTimerMs;
};
#endif // FACEIDENTIFY_H
