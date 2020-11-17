
#ifndef Crc16_h
#define Crc16_h

#include "types.h"

extern void Crc16CcittUpdate(BYTE ucData, WORD *pwCrc);

extern WORD GetCrc16Ccitt(WORD wBase, BYTE* pucBuff, DWORD dwLen);

extern WORD GetCrc16CcittRev(WORD wBase, BYTE* pucBuff, DWORD dwLen);

#endif