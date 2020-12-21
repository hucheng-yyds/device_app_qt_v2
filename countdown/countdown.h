#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#include <QDateTime>

// 定时器
class CountDown
{
public:
    explicit CountDown();
    CountDown(int ms);
    // 是否已到定时的时长，是返回true，否返回false
    bool expired();
    // 设置定时时长。 从当前时间开始的定时时长，单位ms
    void countdown_ms(int ms);

private:
    // 定时到达的结束时间 单位ms
    qint64 m_endTimerMs;
};

#endif // COUNTDOWN_H
