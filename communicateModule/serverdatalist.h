#ifndef FACEDATALIST_H
#define FACEDATALIST_H
#include <QString>
#include <QList>
#include <QMutex>
#include <QJsonObject>

typedef struct {
    QByteArray datas;
} FacePacketNode_t;

// 星河后台mqtt数据缓存 V833考虑性能的问题 需使用二代协议
class ServerDataList {
public:
    ServerDataList();
    ~ServerDataList();
    // mqtt数据入缓存
    void PushLogPacket(FacePacketNode_t* Packet);
    // 从缓存取mqtt数据
    FacePacketNode_t* GetLogPacket();
    // 清空缓存数据
    void ClearLogPacket();
protected:
    QList<FacePacketNode_t *> m_dataList;
    QMutex m_mutex;
};
#endif // FACEDATALIST_H
