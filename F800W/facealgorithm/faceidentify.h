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

private:
    // 处理开门权限
    QStringList dealOpencondition(int faceId);

signals:
    // 人脸检测结果显示
    void faceResultShow(const QString &name, int index, int trackId, const QString &result);

private:
    FaceInterface *m_interFace;
    unsigned char *m_irImage;
    unsigned char *m_bgrImage;
    QVector<MFaceHandle> m_iMFaceHandle;
};
#endif // FACEIDENTIFY_H
