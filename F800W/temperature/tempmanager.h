#ifndef TEMPMANAGER_H
#define TEMPMANAGER_H
#include <QThread>
#include <QTimer>
#include "switch.h"

//通信协议 地址[4]包数[2]长度[2]数据[*]校验[2]
typedef struct {
    unsigned char heard[4];
    unsigned char addr[4];
    unsigned char len[2];
}update_protocol_t;


// 测温管理模块
class TempManager : public QThread
{
    Q_OBJECT
public:
    explicit TempManager();

protected:
    virtual void run();

signals:
    void tempeatureInfo(QByteArray tempInfo);

public slots:
    // 温度模块命令控制接口 -> 温度校准 全屏测温和关全屏测温
    void onSendCmdToTemp(QByteArray tempInfo);
    // 停止对应命令的返回应答数据
    void onRecvStopTemp();
    // 程序上电时获取 如果返回0表示不带测温模块 如果非0表示是测温模块
    float onIsTemp();
    // 测温模块OTA升级开始发送程序数据
    void sendTempProgram();
    // 获取测温模块数据
    void getTempInfo();
    void openAllScreenTemp(bool status);
    // 开始测温计时开始
    void startTemp();
    // 串口缓存清空
    void tcflsh();
    // 停止测温 测温不足1.5 测温数据置0认定测温失败
    void endTemp();

private:
    // 定时器 200毫秒运行依次 检测是否开始取温度数据
    void timeckeck();

private:
    void openTempFile(const QString &name);
    void sendTempFileData();
    int update_write(update_protocol_t *This,unsigned char *dat,unsigned short length,unsigned char *out);
    void recvUpdateData();
    bool checkCRC(QVector<uchar> buff);
    void checkUART();
    // 得到测温模块的返回值
    void getTempReturn();
    // 从测温模组读取测温数据，并计算出温度值
    QString getTemperature();
    // 根据取出的温度值，判断当前的测温数据 返回值0:温度大于预警值 1:温度正常 -1:体温偏低 -2:测温失败
    int compareTemp(const QString &tempVal);

    // 是否已到定时的时长，是返回true，否返回false
    bool expired();
    // 设置定时时长。 从当前时间开始的定时时长，单位ms
    void countdown_ms(int ms);

private:
    // 定时到达的结束时间 单位ms
    qint64 m_endTimerMs;
    int m_fileSize;
    int m_upgradeFileSize;
    char *m_fileBuf;
    bool sendOk = false;
    bool m_recTempDataOK;
    qint64 m_timeTemp;
    bool m_tempStatus;
    QTimer *m_timer;
    int m_tempCount;
    bool m_startTemp;
    float m_tempModule;
    int m_fd;
};
#endif // TEMPMANAGER_H
