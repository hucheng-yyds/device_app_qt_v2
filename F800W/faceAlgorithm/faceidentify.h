#ifndef FACEIDENTIFY_H
#define FACEIDENTIFY_H
#include <QThread>
#include "faceinterface.h"

class FaceIdentify : public QThread
{
    Q_OBJECT
public:
    explicit FaceIdentify();
    // 人脸检测和比对共享资源接口
    void setFaceInter(FaceInterface *inter);

protected:
    virtual void run();

public slots:

private:
    // 人证比对
    bool idCardFaceComparison(char *feature_result);
    // 处理开门权限
    QStringList dealOpencondition(int faceId);

signals:
    // 人脸检测结果显示 ui显示
    void faceResultShow(const QString &name, int index, int trackId, const QString &result);
    // 开始测温信号
    void startTemp();
    // UI显示正在测温
    void showStartTemp();
    // 显示测温状态和结果
    void tempShow(const QString &tempVal, int result);
    // 实时上传记录
    void uploadopenlog(int id, int userId, const QString &photo, int isOver,int type, int isTemp, const QStringList &datas);

private:
    FaceInterface *m_interFace;
    unsigned char *m_irImage;
    unsigned char *m_bgrImage;
    QVector<MFaceHandle> m_iMFaceHandle;
    bool m_tempFlag;
    QString m_tempVal;
    int m_tempResult;
};
#endif // FACEIDENTIFY_H
