#ifndef USERIDREQUEST_H
#define USERIDREQUEST_H
#include <QThread>
#include <QMutex>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "switch.h"
#include "sqldatabase.h"

class UserIdRequest : public QThread
{
    Q_OBJECT
public:
    explicit UserIdRequest();

protected:
    virtual void run();

public slots:
    // 处理所有人员id
    void onAlluserId(const QJsonArray &jsonArr);
    // 处理单个人员数据
    void onUpdateUsers(const QJsonObject &jsonObj);
    // 处理alluserIc
    void onAllUsersIc(const QJsonArray &jsonArr);
    // 处理增量users
    void onNewUsers(const QJsonArray &jsonArr);

signals:
    // 向后台拉取全量IC卡
    void allUserIc();
    // 发送获取单个人员数据
    void getUsers(int id);
    // 发送上传失败记录
    void sigInsertFail();
    // 发送单个人员入库
    void insertFaceGroups(int id, const QString &username, const QString &time, const QString &photoname, const QString &iphone);

private:
    // 更新算法阈值
    void updateIdentifyValue();
    // 星河后台人员字段解析
    void httpsUpdateUsers(const QJsonObject &jsonObj);
    // 星云后台人员字段解析
    void tcpUpdateUsers(const QJsonObject &jsonObj);

private:
    QMutex m_mutex;
    QSet<int> m_updateFace;
    bool m_startFaceDownload;
    bool m_faceSyncStatus;
    int m_curFaceId;
};

#endif // USERIDREQUEST_H
