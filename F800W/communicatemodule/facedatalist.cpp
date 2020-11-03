#include "facedatalist.h"

FaceDataList::FaceDataList()
{
    m_dataList.clear();
}
FaceDataList::~FaceDataList()
{
    m_mutex.lock();
    QList<FacePacketNode_t*>::iterator iter = m_dataList.begin();
    for ( ; iter != m_dataList.end(); iter++)
    {
        delete (*iter);
    }
    m_mutex.unlock();
    m_dataList.clear();
}

void FaceDataList::PushLogPacket(FacePacketNode_t* Packet)
{
    if(!Packet)
    {
        return;
    }
    m_mutex.lock();
    m_dataList.push_front(Packet);
    m_mutex.unlock();
}

FacePacketNode_t* FaceDataList::GetLogPacket()
{
    if(!m_dataList.empty())
    {
        FacePacketNode_t *packet = new FacePacketNode_t;
        FacePacketNode_t *pkt = m_dataList.back();
        packet->datas = pkt->datas;
        delete pkt;
        pkt = nullptr;
        m_dataList.pop_back();
        return packet;
    }
    return nullptr;
}

void FaceDataList::ClearLogPacket()
{
    m_mutex.lock();
    if(!m_dataList.empty())
    {
        QList<FacePacketNode_t*>::iterator iter = m_dataList.begin();
        for ( ; iter != m_dataList.end(); iter++)
        {
            delete (*iter);
            *iter = nullptr;
        }
    }
    m_dataList.clear();
    m_mutex.unlock();
}
