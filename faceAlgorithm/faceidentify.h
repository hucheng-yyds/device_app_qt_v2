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
    // 人证比对
    bool idCardFaceComparison(char *feature_result);
    // 处理开门权限
    QStringList dealOpencondition(int faceId);
    // 根据时间切换播报问候语
    void judgeDate();

signals:
    // 韦根输出
    void wgOut(const QByteArray &number);
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
    bool m_tempFlag;
    QString m_tempVal;
    QString m_faceInfo;
    int m_wavSeq;
    int m_tempResult;
    QString m_cardNo;
    bool m_cardWork;
    qint64 m_endTimerMs;

};
#endif // FACEIDENTIFY_H
