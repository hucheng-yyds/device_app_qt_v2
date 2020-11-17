#ifndef __SPICARD_H__
#define __SPICARD_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 


typedef enum _CARD_Type{
  UNDEFINED_C_TYPE=0,
  C_TYPE_14443A0,     //1  UUID = 4
  C_TYPE_14443A1,     //2  UUID = 7
  C_TYPE_14443A2,     //3  CPU卡
  C_TYPE_14443A3,     //4  Apdu手机
  C_TYPE_14443B,      // 5  身份证
  C_TYPE_14443B_ST25TB,//6  ST25TB
  C_TYPE_15693,       //7  15693
  C_TYPE_FELICA,      //8  菲林卡
  C_MAX
}CARD_Type;

typedef struct _CardInfoType{
  char uCardId[23];
  char uCardIdLen;
  char uATQA,uSAK;
  CARD_Type TagType;
}CardInfoType;

int IF_InitSPICard(void);
int IF_GetCardId(CardInfoType* buf);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif // End of 

