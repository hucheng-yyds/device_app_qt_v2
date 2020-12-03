#ifndef PROSTORAGE_H
#define PROSTORAGE_H
#include <QThread>
#include "switch.h"
#include "faceidentify.h"
#include "facemanager.h"
#include "netmanager.h"
#include "httpsclient.h"
#include "tcpclient.h"
#include "tempmanager.h"
#include "httpsclient.h"
#include "useridrequest.h"
#include "mqttmodule.h"
#include "serverdatadeal.h"
#include "serverdatalist.h"
#include "ToolTcpServer.h"
#include "idmodule.h"
#include "icmodule.h"
#include "offlinerecord.h"
#include "wgmodule.h"

// 公共接口层 ui界面接口层
class ProStorage : public QThread
{
    Q_OBJECT
public:
    explicit ProStorage();
    // 生成二维码
    void saveQRcodeImage(const char *content);

protected:
    void run();

signals:
    // ui界面更新时间
    void timeSync(int sleeptime, bool sleepctl, const QString &dataCur, const QString &digitalClock, int hour, int min, const QString &dataTime);
    // 人脸检测结果显示 ui显示
    void faceResultShow(const QString &name, int index, int trackId, const QString &result, const QString &showInfo);
    // 显示人脸框 ui界面显示
    void showFaceFocuse(int left, int top, int right, int bottom, int index, int trackId);
    // 隐藏人脸框 控制ui界面
    void hideFaceFocuse();
    // 发送信号 ui界面显示版本号 设备名称 同步人员个数 设备ip 设备sn码
    void showDeviceInfo(bool tempCtl, bool faceCtl, const QString &ver, const QString &name, const QString &number, const QString &devIp, const QString &devSn);
    // 发送授权状态 和显示授权页面 UI界面显示
    void qrcodeChanged(const QString &rcode);
    // 刷新ui界面 初始化完成
    void syncSuccess(bool eg, bool temp);
    // UI显示正在测温
    void showStartTemp();
    // 显示测温状态和结果
    void tempShow(const QString &tempVal, int result);
    // 切换网络状态
    void networkChanged(int type, bool net);
    // 人员同步和升级状态显示
    void faceTb(const QString &text);
    // ic状态显示
    void icResultShow(int result, const QString &cardNo, const QString &showInfo);
    // 显示身份证信息
    void idCardResultShow(int flag, const QString &name, const QString &result, const QString &showInfo);
    // 读卡状态
    void readIcStatus(int flag);
    // 口罩和安全帽显示
    void maskHelmet(int flag);

private:
    // 功能模块初始化
    void init();
    // 判断sn码是否产生
    void DeviceSnJudgment();

private:

};

#endif // PROSTORAGE_H
