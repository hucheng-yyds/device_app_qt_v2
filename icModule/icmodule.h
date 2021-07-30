#ifndef ICMODULE_H
#define ICMODULE_H
#include <QThread>
#include "switch.h"
#include "qalhardware.h"
#include "sqldatabase.h"

class IcCardModule : public QThread
{
    Q_OBJECT
public:
    IcCardModule();

protected:
    virtual void run();

signals:
    // 读卡状态
    void readIcStatus(int flag);
    // 处理ic数据
    void sigIcInfo(int mid, const QString &cardNo);
private:
    // 是否已到定时的时长，是返回true，否返回false
    bool icExpired();
    // 设置定时时长。 从当前时间开始的定时时长，单位ms
    void icCountdown_ms(int ms);

private:
    // 定时到达的结束时间 单位ms
    qint64 m_endTimerMs;
    QByteArray m_icDatas;
    int m_fd;
};
#endif // ICMODULE_H
