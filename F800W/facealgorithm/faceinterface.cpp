#include "faceinterface.h"

FaceInterface::FaceInterface()
{
    m_iFaceHandle = nullptr;
    m_count = 0;
    m_faceHandle.clear();
    m_iStop = true;
    m_quality = true;
    m_irImage = new unsigned char[VIDEO_WIDTH*VIDEO_HEIGHT* 3 / 2];
    m_bgrImage = new unsigned char[VIDEO_WIDTH*VIDEO_HEIGHT* 3 / 2];
}
