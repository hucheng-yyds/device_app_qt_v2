#ifndef PROSTORAGE_H
#define PROSTORAGE_H
#include <QThread>
#include "switch.h"
#include "faceidentify.h"
#include "facemanager.h"
#include "netmanager.h"

// 公共接口层 ui界面接口层
class ProStorage : public QObject
{
    Q_OBJECT
public:
    explicit ProStorage();

signals:
    // 人脸检测结果显示
    void faceResultShow(const QString &name, int index, int trackId, const QString &result);
    // 显示人脸框
    void showFaceFocuse(int left, int top, int right, int bottom, int index, int trackId);
    // 隐藏人脸框
    void hideFaceFocuse();
    // 发送信号 ui界面显示版本号 设备名称 同步人员个数 设备ip 设备sn码
    void showDeviceInfo(const QString &ver, const QString &name, const QString &number, const QString &devIp, const QString &devSn);

private:
    // 功能模块初始化
    void init();

private:

};

#endif // PROSTORAGE_H
