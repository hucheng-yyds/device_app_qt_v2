#ifndef FACEDATADEAL_H
#define FACEDATADEAL_H
#include <QThread>
#include <QJsonArray>
#include <QJsonObject>
#include "mqttclient.h"
#include "sqldatabase.h"
#include "httpsclient.h"
#include "sample_vio.h"
#include "qalhardware.h"

// 处理后台服务器数据
class ServerDataDeal : public QThread
{
    Q_OBJECT
public:
    explicit ServerDataDeal();
    // 处理后台服务器数据入口
    void setPacket(ServerDataList *packet);
    // http图片下载接口
    void setHttp(HttpsClient *httpClient);

protected:
    void run();

signals:
    // 服务器应答
    void responseServer(const QJsonObject &jsonData);
    // 处理增量users
    void newUsers(const QJsonArray &jsonArr);
    // 发送失败人脸入库
    void sigInsertFail();
    // 发送单个人员入库
    void insertFaceGroups(int id, const QString &username, const QString &time, const QString &photoname, const QString &iphone);

private:
    // 软件升级
    void upgradeFile(const QJsonObject &obj);
    // 配置修改
    void saveSetting(const QJsonObject &jsonData);
    // 处理后台数据
    void dealJsonData(QJsonObject jsonObj);
    // 处理人脸变动数据
    void dealFaceNewData(QJsonObject jsonObj);

    void faceInsertSql(int id, int passnum, int idBack, const QStringList &data);

private:
    bool m_status;
    ServerDataList *m_packet;
    HttpsClient *m_httpsClient;
};
#endif // FACEDATADEAL_H
