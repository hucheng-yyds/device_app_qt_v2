#ifndef NETMANAGER_H
#define NETMANAGER_H
#include <QThread>
#include "switch.h"
#include "sqldatabase.h"

class NetManager : public QThread
{
    Q_OBJECT
public:
    explicit NetManager();

protected:
    virtual void run();

signals:
    // 发送信号 ui界面显示版本号 设备名称 同步人员个数 设备ip 设备sn码
    void showDeviceInfo(const QString &ver, const QString &name, const QString &number, const QString &devIp, const QString &devSn);
    // 切换网络状态
    void networkChanged(int type, bool net);

private slots:


private:
    QString getIP();

private:
};
#endif // NETMANAGER_H
