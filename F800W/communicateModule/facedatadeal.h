#ifndef FACEDATADEAL_H
#define FACEDATADEAL_H
#include <QThread>
#include <QJsonArray>
#include <QJsonObject>
#include "mqttclient.h"
#include "switch.h"
#include "sqldatabase.h"
#include "httpsclient.h"

// 处理星河 mqtt数据
class FaceDataDeal : public QThread
{
    Q_OBJECT
public:
    explicit FaceDataDeal();
    // 处理mqtt数据入口
    void setPacket(FaceDataList *packet);
    // http图片下载接口
    void setHttp(HttpsClient *httpClient);

protected:
    void run();

signals:
    // 发送失败人脸入库
    void sigInsertFail();
    // 发送单个人员入库
    void insertFaceGroups(int id, const QString &username, const QString &time, const QString &photoname, const QString &iphone);

private:
    void dealJsonData(QJsonObject jsonObj);
    void dealFaceNewData(QJsonObject jsonObj);
    void faceInsertSql(int id, int passnum, int idBack, const QStringList &data);

private:
    bool m_status;
    FaceDataList *m_packet;
    HttpsClient *m_httpsClient;
};
#endif // FACEDATADEAL_H
