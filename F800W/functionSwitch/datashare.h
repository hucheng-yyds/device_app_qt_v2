#ifndef DATASHARE_H
#define DATASHARE_H
#include <QObject>

#define dataShare DataShare::getInstance()

class DataShare
{
public:
    explicit DataShare();
    static DataShare *getInstance()
    {
        if (!m_Instance) {
            m_Instance = new DataShare;
        }
        return m_Instance;
    }
    // 刷身份证成功标志
    bool m_idCardFlag;
    // 身份证数据
    QStringList m_idCardDatas;
    // 测温结束标志
    bool m_tempFlag;
    // 测温数值
    QString m_tempVal;
    // 测温比对结果
    int m_tempResult;
    // 离线记录上传标志
    bool m_offlineFlag;
    // 同步中标志
    bool m_sync;
    // 升级中标志
    bool m_upgrade;
    // 网路在线状态标志
    bool m_netStatus;
    // 人脸比对阈值 根据入库人数自动调节
    double m_faceThreshold;
    // 设备ip
    QString m_ipAddr;
    //当前日志模块的输出方向，true:输出到文件,false:输出到配置工具
    bool m_log;

private:
    static DataShare *m_Instance;
};
#endif // DATASHARE_H
