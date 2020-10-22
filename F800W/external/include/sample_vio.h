#ifndef __SAMPLE_VIO_H__
#define __SAMPLE_VIO_H__

#include "hi_common.h"
#include "hi_comm_video.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

HI_S32 IF_VIO_VPSS_VO_MIPI_TX(HI_U32 u32VoIntfType,HI_U32 u32VoscreenType,HI_U32 u32ChnRotation);
int IF_CheckYUVDataReady(unsigned short nCh);
int IF_GetData(VIDEO_FRAME_INFO_S ** lpVideoFrame,int w,int h);
int IF_GetIRData(VIDEO_FRAME_INFO_S ** lpVideoFrame,int w,int h);
int IF_GetData_YUV_1080P(HI_VOID);
int IF_ReleaseData(VIDEO_FRAME_INFO_S * lpVideoFrame);
int IF_ReleaseIRData(VIDEO_FRAME_INFO_S * lpVideoFrame);
HI_S32 IF_AUDIO_Init(HI_VOID);
HI_S32 IF_PCMAUDIO_Init(HI_VOID);
HI_S32 IF_AUDIO_SendData(int len,char* pu8AudioStream);
HI_S32 IF_PCMAUDIO_SendData(int len,char* pu8AudioStream, int ind);
HI_S32 IF_Vol_Set(HI_S32 vol);
HI_S32 IF_Vol_Get(HI_S32 *vol);
HI_S32 IF_AUDIO_ClearBuf(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef __SAMPLE_VIO_H__*/
