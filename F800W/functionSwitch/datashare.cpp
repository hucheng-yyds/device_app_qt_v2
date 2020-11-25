#include "datashare.h"

DataShare* DataShare::m_Instance = nullptr;

DataShare::DataShare()
{
    m_faceThreshold = 72.0;
    m_idCardFlag = false;
    m_idCardDatas.clear();
    m_tempFlag = false;
    m_sync = false;
    m_upgrade = false;
    m_netStatus = false;
    m_offlineFlag = false;
    m_log = true;
}
