#ifndef __SAMPLE_VIO_H__
#define __SAMPLE_VIO_H__

#include "mm_comm_vo.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

int IF_VIO_VPSS_VO_MIPI_TX(int u32VoIntfType,int u32VoscreenType,int u32ChnRotation);
int IF_CheckYUVDataReady(unsigned short nCh);
int IF_GetData(VIDEO_FRAME_INFO_S ** lpVideoFrame,int w,int h);
int IF_GetIRData(VIDEO_FRAME_INFO_S ** lpVideoFrame,int w,int h);
int IF_ReleaseData(VIDEO_FRAME_INFO_S * lpVideoFrame);
int IF_ReleaseIRData(VIDEO_FRAME_INFO_S * lpVideoFrame);
int IF_AUDIO_Init(void);
int IF_PCMAUDIO_Init(void);
int IF_AUDIO_SendData(int len,char* pu8AudioStream);
int IF_PCMAUDIO_SendData(int len,char* pu8AudioStream, int ind);
int IF_Vol_Set(int vol);
int IF_Vol_Get(int *vol);
int IF_AUDIO_ClearBuf(void);
int IF_PCMAUDIO_ClearBuf(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __SAMPLE_VIO_H__*/
