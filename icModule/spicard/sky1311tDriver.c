
#include "sky1311tDriver.h"

//公共函数
static void Sky1311DrvInit(CModule oM){
	CSky1311 oNfc= (CSky1311)oM;
	//memset(oNfc->pucTransBuff, 0, oNfc->pConst->wBuffSize);
	//memset(oNfc->pucTagUid, 0, TAG_NUM_SIZE_MAX);
	//memset(oNfc->pucTagLastUid, 0, TAG_NUM_SIZE_MAX);
	//memset(oNfc->NfcIdNum, 0, TAG_NUM_SIZE_MAX);
	oNfc->ucReadLen= TAG_READ_SIZE_DEF;
        oNfc->wRcValue=0;
        oNfc->uRcAdcValue=0;
        oNfc->bhasCardIn=FALSE;
        oNfc->bcardReadOver=FALSE;
	oNfc->Mode= SKY1311_WKM_DEFAULT;
	//oNfc->ucCardSel= ST95_CARD_SEL_DEF;
	oNfc->w1msCnt= 0;
	if (oNfc->pApi->pObj== NULL){
		//API接口的外部调用对象句柄未赋值，则赋为本模块句柄
		oNfc->pApi->pObj= oM;
	}
	oNfc->Status= SKY1311_INIT;
}
/*
static BOOL Sky1311DrvReadCard(CModule oM, BYTE TagType){

        CSky1311 oNfc= (CSky1311)oM;
  
	BYTE* pucData= malloc(oNfc->ucReadLen);
	if (pucData!= NULL){
        BYTE ret = ConfigManagerTagRead(pucData, oNfc->ucReadLen, 0, oNfc);
		if (ret == 0){
			if (oNfc->pApi->ApiSt95hfReadCallback != NULL){
				oNfc->pApi->ApiSt95hfReadCallback(
					oNfc->pApi->pObj,
					oNfc->pucTagUid, 
					pucData,
					&oNfc->ucReadLen,
					&TagType
				);
				free(pucData);
				return RET_SUCCESS;
			}
		}
    else if(TagType == TRACK_NFCTYPEMIFARE) {  //只反卡号
      if (oNfc->pApi->ApiSt95hfReadCallback != NULL){
				oNfc->pApi->ApiSt95hfReadCallback(
					oNfc->pApi->pObj,
					oNfc->pucTagUid, 
					NULL,
					0,
					&TagType
				);	
      }
    }
	}
  free(pucData);
  
	return RET_FAILURE;
}
*/
//---------------------------------------------------------------------
//			SKY1311读卡函数
//功能：
//	用于在卡片工作时读取卡片的数据，必须在ApiSky1311ReadCallback()
//	回调函数中使用
//参数：
//	oM：SKY1311驱动层的句柄
//	pucBuff：输出读到的的卡片数据
//	ucLen：数据长度
//	wOffset：数据的起始位置
//返回：
//	RET_SUCCESS：读取成功，RET_FAILURE：读取失败
//---------------------------------------------------------------------
BOOL ApiSky1311ReadCard(
	CSky1311 oNfc,
	BYTE* pucBuff, 
	BYTE ucLen, 
	WORD wOffset)
{	
	return FALSE;//ConfigManagerTagRead(pucBuff, ucLen, wOffset, oNfc)==0?RET_SUCCESS:RET_FAILURE;
}

//---------------------------------------------------------------------
//			SKY1311校准唤醒灵敏度的函数
//功能：
//	用于获取唤醒的阈值，该阈值在本层保存。初始化时自动执行，也可以
//	在需要的时候执行
//参数：
//	oM：SKY1311驱动层的句柄
//返回：
//	无
//---------------------------------------------------------------------
void ApiSky1311Calibration(CSky1311 oNfc){
	//BYTE ucCal= PCD_TagDetectCalibration(WU_PERIOD, &(oNfc->ucDacDataH),oNfc);
	//Printf("Calibration=%d\r\n",ucCal);
	//Printf("DacDataH=%d\r\n",oNfc->ucDacDataH);	
	//PCD_WaitforTagDetection(WU_SOURCE, WU_PERIOD, oNfc->ucDacDataH, NB_TRIALS, oNfc);
	oNfc->Status= SKY1311_DET;
}

//---------------------------------------------------------------------
//			SKY1311设置读卡数据长度的函数
//功能：
//	用于设置回调函数ApiSky1311ReadCallback()读卡的数据长度
//参数：
//	oM：SKY1311驱动层的句柄
//	ucLen：数据长度
//返回：
//	无
//---------------------------------------------------------------------
void ApiSky1311SetReadLen(CSky1311 oNfc, BYTE ucLen){
	oNfc->ucReadLen= ucLen;
}

//---------------------------------------------------------------------
//			SKY1311启用和停用的函数
//功能：
//	用于设置SKY1311启用和停用，停用后读卡不起作用
//参数：
//	oM：SKY1311驱动层的句柄
//	bStop：FALSE：启用，TRUE：停用
//返回：
//	无
//---------------------------------------------------------------------
void ApiSky1311StartStop(CSky1311 oNfc, BOOL bStop){
	if (bStop== TRUE){
		oNfc->Status= SKY1311_IDLE;
	}
	else{
		//PCD_WaitforTagDetection(WU_SOURCE, WU_PERIOD, oNfc->ucDacDataH, NB_TRIALS, oNfc);
		oNfc->Status= SKY1311_DET;
	}
}

//---------------------------------------------------------------------
//			SKY1311设定使用的卡类型的函数
//功能：
//	用于设定使用的卡类型
//参数：
//	oNfc：SKY1311驱动层的句柄
//	ucSel：开放的卡类型：
//		NOTHING		0x00 // 0000 0000
//		NFCTYPE1 	0x01 // 0000 0001 
//		NFCTYPE2 	0x02 // 0000 0010 
//		NFCTYPE3 	0x04 // 0000 0100 
//		NFCTYPE4A 	0x08 // 0000 1000 
//		NFCTYPE4B 	0x10 // 0001 0000 
//		NFCTYPE5 	0x20 // 0010 0000 
//		NFCTYPEMIFARE	0x40 // 0100 0000
//		NFC_PHONE 	0x80 // 1000 000
//		ALL 		0xFF // 1111 1111 
//返回：
//	无
//---------------------------------------------------------------------
void ApiSky1311CardSelect(CSky1311 oNfc, BYTE ucSel){
	//oNfc->ucCardSel= ucSel;
}

//---------------------------------------------------------------------
//			ST95HF设定工作模式的函数
//功能：
//	用于设置ST95HF的工作模式
//参数：
//	oM：ST95HF驱动层的句柄
//	Mode：	ST95_WKM_SLEEP：周期性唤醒读卡模式，
//		ST95_WKM_CONTINUE：连续模式
//返回：
//	无
//---------------------------------------------------------------------
void ApiSky1311ModeSelect(CSky1311 oNfc, Sky1311WorkModeType Mode){
	oNfc->Mode= Mode;
	if (Mode== SKY1311_WKM_SLEEP){
		//PCD_WaitforTagDetection(WU_SOURCE, WU_PERIOD, oNfc->ucDacDataH, NB_TRIALS, oNfc);
		oNfc->Status= SKY1311_DET;
	}
	else{
		oNfc->pConst->ClrWakeupPin();
		oNfc->pConst->Delay(1);		
		oNfc->pConst->SetWakeupPin();
		oNfc->Status= SKY1311_READ;
	}
}
/*
static BOOL MX_sky1311t_Init(CSky1311 oNfc)
{
    // RC 频率校准，获得最接近13.56M的RC值以及对应的AD值 /
    oNfc->wRcValue = sky1311RCFreqCali(pSky);
    if(pSky->pVar->wRcValue == 0)        // 没有采集到合适的AD值
    {
      return true;
    }
    return false;
}
*/
static void Sky1311DrvLoopHandler(CModule oM){
	CSky1311 oNfc= (CSky1311)oM;
	switch (oNfc->Status){
		//初始化
		case SKY1311_INIT:
                    printf("SKY1311 Card Reader Init\r\n");
                    /*
			if(MX_sky1311t_Init(gpSky1311t)== TRUE){
				//ST95HF初始化错误！	
				PutStr("ST95HF HW Init Error\r\n");
				oNfc->Status= SKY1311_IDLE;
			}
			else{
				PutStr("ST95HF HW Init Success\r\n");
				if (oNfc->Mode== SKY1311_WKM_SLEEP){
					oNfc->Status= SKY1311_CALI;
				}
				else{
					oNfc->Status= SKY1311_READ;
				}
			}
                    */
		break;
		//空闲
		case SKY1311_IDLE:
		break;
		//校准
		case SKY1311_CALI:{
			//BYTE ucCal= PCD_TagDetectCalibration(WU_PERIOD, &(oNfc->ucDacDataH),oNfc);
			//oNfc->ucDacDataH= oNfc->ucDacDataH;
			//Printf("Calibration=%d\r\n",ucCal);
			//Printf("DacDataH=%d\r\n",oNfc->ucDacDataH);	
      //if (oNfc->Mode== ST95_WKM_SLEEP){
				oNfc->Status= SKY1311_IDLE;
			//}
			//oNfc->Status= ST95_READ;
		}
		break;
		//卡检测
		case SKY1311_DET:{
			if (oNfc->pConst->GetCardDet!= NULL){
				if (oNfc->pConst->GetCardDet()== ST_LOW){
					oNfc->pConst->ClrWakeupPin();
					oNfc->pConst->Delay(1);
					oNfc->pConst->SetWakeupPin();
					oNfc->Status= SKY1311_READ;
				}
			}
			else{
				oNfc->Status= SKY1311_READ;
			}
		}
		break;
		//读卡
		case SKY1311_READ:{
			/*
                        BOOL bRet= RET_SUCCESS;
			BYTE TagType= ConfigManager_TagHunting(oNfc->ucCardSel, oNfc);
			switch(TagType){
				case TRACK_NFCTYPE4B:{
					PutStr("TAG:ISO14443B\r\n");
					if (oNfc->pApi->ApiSt95hfReadCallback!= NULL){
						BYTE ucLen= 0;
						oNfc->pApi->ApiSt95hfReadCallback(
							oNfc->pApi->pObj,
							oNfc->pucTagUid, 
              NULL,
							&ucLen,
							&TagType);
					}
				}
				break;
				case TRACK_NFCTYPE4A:{
					PutStr("TAG:ISO14443A\r\n");	
					bRet= St95hfDrvReadCard(oM, TagType);
				}				
				break;
				case TRACK_NFCTYPEMIFARE:{
					PutStr("TAG:MIFARE\r\n");
					bRet= St95hfDrvReadCard(oM, TagType);
				}
				break;		
				case TRACK_NFCTYPE5:{
					PutStr("TAG:ISO15693\r\n");
					bRet= St95hfDrvReadCard(oM, TagType);
				}	
				break;
				default:
					bRet= RET_FAILURE;
				break;
			}
			PCD_FieldOff(oNfc);	
			if (bRet== RET_SUCCESS){
				
			}
			if ((oNfc->pConst->GetCardDet!= NULL)&&
				(oNfc->Mode== ST95_WKM_SLEEP))
			{
				//定义了卡检测脚才进入卡检测状态
				//PCD_WaitforTagDetection(WU_SOURCE, WU_PERIOD, oNfc->ucDacDataH, NB_TRIALS, oNfc);
				oNfc->Status= ST95_DET;
			}
                  */
		}
		break;
	}
}

static void IsrSky1311Drv1msTimebase1(CModule oM){
	CSky1311 oNfc= (CSky1311)oM;
	if (oNfc->w1msCnt){
		oNfc->w1msCnt--;
	}
}
