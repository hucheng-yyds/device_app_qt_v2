#include "serverdatalist.h"
#include <QDebug>

ServerDataList::ServerDataList()
{
    m_dataList.clear();
}
ServerDataList::~ServerDataList()
{
    m_mutex.lock();
    QList<FacePacketNode_t*>::iterator iter = m_dataList.begin();
    for ( ; iter != m_dataList.end(); iter++)
    {
        delete (*iter);
    }
    m_dataList.clear();
    m_mutex.unlock();
}

void ServerDataList::PushLogPacket(FacePacketNode_t* Packet)
{
    if(!Packet)
    {
        return;
    }
    m_mutex.lock();
    m_dataList.push_front(Packet);
    m_mutex.unlock();
}

FacePacketNode_t* ServerDataList::GetLogPacket()
{
    m_mutex.lock();
    if(!m_dataList.empty())
    {
        FacePacketNode_t *packet = new FacePacketNode_t;
        FacePacketNode_t *pkt = m_dataList.back();
        packet->datas = pkt->datas;
        delete pkt;
        pkt = nullptr;
        m_dataList.pop_back();
        m_mutex.unlock();
        return packet;
    }
    m_mutex.unlock();
    return nullptr;
}

void ServerDataList::ClearLogPacket()
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
