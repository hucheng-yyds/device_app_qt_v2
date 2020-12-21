
#include "Crc16.h"

void Crc16CcittUpdate(BYTE ucData, WORD *pwCrc){
	WORD x = *pwCrc;
	x  = (WORD)((BYTE)(x >> 8) | (x << 8));
	x ^= (BYTE) ucData;
	x ^= (BYTE)(x & 0xFF) >> 4;
	x ^= (x << 8) << 4;
	x ^= ((x & 0xFF) << 4) << 1;
	*pwCrc = x;
}

WORD GetCrc16Ccitt(WORD wBase, BYTE* pucBuff, DWORD dwLen){
	WORD wRet= wBase;
	DWORD dwCnt= 0;
	for (; dwCnt< dwLen; dwCnt++){
		Crc16CcittUpdate(pucBuff[dwCnt], &wRet);
	}
	return wRet;
}

WORD GetCrc16CcittRev(WORD wBase, BYTE* pucBuff, DWORD dwLen){
	WORD wRet= wBase;
	while(dwLen--){
		Crc16CcittUpdate(pucBuff[dwLen], &wRet);
	}
	return wRet;
}
