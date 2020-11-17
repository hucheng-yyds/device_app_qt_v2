#ifndef WGMODULE_H
#define WGMODULE_H
#include <QThread>
#include <sys/ioctl.h>
#include "switch.h"
#include "sqldatabase.h"

#define WG_CMD_MAGE     'W'
#define WG_DATA_R       0
#define WG_DATA_W       1
#define WG_DATA_WR      _IOR(WG_CMD_MAGE, 3, char)
struct weigen_d{
    int type;              //  26 34 34A
    char buf[8];          //  data
};

class WgModule : public QThread
{
    Q_OBJECT
public:
    explicit WgModule();

protected:
    virtual void run();

public slots:
    void wgOutuChar(int type, char *buf);
    void wgOut(QByteArray number);

signals:
    void sigWgInfo(int mid, const QString &cardNo);

private:
    // 是否已到定时的时长，是返回true，否返回false
    bool wgExpired();
    // 设置定时时长。 从当前时间开始的定时时长，单位ms
    void wgCountdown_ms(int ms);

private:
    int m_fd = 0;
    // 定时到达的结束时间 单位ms
    qint64 m_endTimerMs;
};

#endif // WGMODULE_H
