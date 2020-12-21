#ifndef HTTPSCLIENT_H
#define HTTPSCLIENT_H
#include <QThread>
#include <QtNetwork>
#include "switch.h"
#include "sqldatabase.h"

// http后台网络请求
class HttpsClient : public QThread
{
    Q_OBJECT
public:
    explicit HttpsClient();
    static int httpsQRCode(const QString &data);
    static bool httpsUserReq(int id, const QString &url);
    // 获取算法授权状态
    int AlgorithmAuthorization();
    // 获取人脸图片
    int httpsRequestImage(int id, const QString &url);
    // ota下载
    void httpsDownload(const QString &url);

public slots:
    // 根据人员id获取人员数据
    void HttpsGetUsers(int id);
    // 实时上传记录
    void httpsUploadopenlog(int id, int userId, const QString &photo, int isOver,int type, int isTemp, int sex, const QStringList &datas);
    // 解绑后台
    void httpsUnbind();

protected:
    virtual void run();

private slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void HttpLogin();
    void HttpHeartbeat();

signals:
    // 发送授权状态 和显示授权页面 UI界面显示
    void qrcodeChanged(const QString &rcode);
    // 处理所有alluserid
    void allUserId(const QJsonArray &jsonArr);
    // 发送单个人员数据
    void updateUsers(const QJsonObject &jsonObj);
    // 发送mqtt连接信号
    void mqttReconnect();

private:
    // http拉取全量数据
    void HttpsGetAllUserId();
    // 算法授权http请求
    QJsonObject requestAuth(const QString &url, const QJsonObject &jsonObj);
    // http get请求数据
    QJsonObject requestGet(const QString &url);
    // http post请求数据
    QJsonObject requestPost(const QString &url, const QJsonObject &jsonObj);
    // http post心跳包请求数据
    QJsonObject requestPostHeart(const QString &url, const QJsonObject &jsonObj);

private:
    QString m_strKey;
    QTimer *m_heartbeatTimer;
    QTimer *m_loginTimer;
    QTimer *m_requestAllowTimer;
    // 是否运行发送数据命令
    bool m_allowRequest;
    bool m_loginStatus;
    // 后台地址
    QString m_httpAddr;
    // 获取token命令
    QString m_tokenCmd;
    // 发送心跳包命令
    QString m_heartbeatCmd;
    // 请求单条人员命令
    QString m_getuserCmd;
    // 上传通行命令字段
    QString m_uploadAccessCmd;
    // 拉取全量命令
    QString m_alluserCmd;
    // ota升级命令
    QString m_newupgrade;
    // 解绑命令
    QString m_unbindCmd;
    // 上传人员入库失败命令
    QString m_uploadFailFace;
    // 组织id
    QString m_companyId;
    // 场景id
    QString m_sceneId;
    // 是否上传成功
    bool m_uploadStatus;
};
#endif // HTTPSCLIENT_H
