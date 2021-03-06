#include "faceinterface.h"

FaceInterface::FaceInterface()
{
    m_ptrAppData = nullptr;
    m_count = 0;
    m_trackId = -1;
    m_success = false;
    m_faceHandle.clear();
    m_iStop = true;
    m_quality = true;
    m_localFaceSync = false;
    m_irImage = new unsigned char[SOURCE_WIDTH*SOURCE_HEIGHT* 3 / 2];
    m_bgrImage = new unsigned char[VIDEO_WIDTH*VIDEO_HEIGHT* 3];
}
