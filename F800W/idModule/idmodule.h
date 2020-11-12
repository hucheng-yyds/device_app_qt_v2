#ifndef IDMODULE_H
#define IDMODULE_H
#include <QThread>
#include "idInclude.h"
#include "switch.h"
#include "qalhardware.h"

class IdCardModule : public QThread
{
    Q_OBJECT
public:
    IdCardModule();

protected:
    virtual void run();

signals:

private:
    // 是否已到定时的时长，是返回true，否返回false
    bool expired();
    // 设置定时时长。 从当前时间开始的定时时长，单位ms
    void countdown_ms(int ms);

private:
    // 定时到达的结束时间 单位ms
    qint64 m_endTimerMs;
};
#endif // IDMODULE_H
