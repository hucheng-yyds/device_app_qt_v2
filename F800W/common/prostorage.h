#ifndef PROSTORAGE_H
#define PROSTORAGE_H
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>
#include "switch.h"
#include "faceidentify.h"
#include "facemanager.h"

// 公共接口层 ui界面接口层
class ProStorage : public QThread
{
    Q_OBJECT
public:
    explicit ProStorage();

protected:
    virtual void run();

signals:
    // 人脸检测结果显示
    void faceResultShow(const QString &name, int index, int trackId, const QString &result);
    // 显示人脸框
    void showFaceFocuse(int left, int top, int right, int bottom, int index, int trackId);

private:
    // 功能模块初始化
    void init();

private:

};

#endif // PROSTORAGE_H
