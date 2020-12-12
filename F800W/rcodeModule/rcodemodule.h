#ifndef RCODEMODULE_H
#define RCODEMODULE_H
#include <QThread>

class RcodeModule : public QThread
{
    Q_OBJECT
public:
    explicit RcodeModule();

protected:
    virtual void run();

public slots:
    void recvRcodeResult(const QByteArray &results);

signals:
    // 实时上传记录
    void uploadopenlog(int id, int userId, const QString &photo, int isOver,int type, int isTemp, int sex, const QStringList &datas);
    // ic状态显示
    void rcodeResultShow(int result, const QString &cardNo, const QString &showInfo);

private:
    // 是否已到定时的时长，是返回true，否返回false
    bool expired();
    // 设置定时时长。 从当前时间开始的定时时长，单位ms
    void countdown_ms(int ms);

private:
    QByteArray m_rcodeDatas;
    int m_fd;
    qint64 m_endTimerMs;
};
#endif // RCODEMODULE_H
