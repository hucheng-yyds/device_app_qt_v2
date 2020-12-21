
#include "userdef.h"
#include "sky1311t.h"
#include "iso14443.h"
#include "string.h"
#include "mcu.h"
#include "sky1311Instan.h"
//#include "mcu.h"
//#include "menKouJi.h"

/* 按照字符串输出10进制数 */
void Dec2Str(u32 DecNum, BYTE *DecStr, BYTE *size)
{
    BYTE temp[22];
    BYTE i=0;
    *size = 0;
    do{
        temp[i++] = (BYTE)(DecNum%10)+0x30;
        DecNum/=10;

    }while(DecNum>0);
    *size = i;

    for(i=0;i<*size;i++)
        DecStr[i] = temp[(*size)-i-1];

}

/***************************************************************
    M1卡读取前5个扇区操作示例
   读取扇区数据并通过串口打印出来
   寻卡等操作时，显示操作结果
   有错误时显示错误信息
****************************************************************/
#if 0
BYTE mifare1_readDemo(PSky1311t pSky)
{
    sta_result_t sta;
    BYTE tmpBuf[20];
    BYTE UID[10];
    BYTE default_key[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    BYTE sector_num, block_num;

    SKY1311_ENABLE(); 	    // chip enable PD2
    DelayMS(1);
    sky1311Init();			// init sky1311s
    resetPicc();            // 复位卡片
    typeAOperate();
    sta = piccRequestA(tmpBuf);
    if(Ok == sta)
    {
        DrvPutStr("\r\nRead Mifare 1 test\r\n");
        DrvPrintf("ATQA:%d",tmpBuf[0]);
    }
    else
    {
        sky1311Reset();
        return 1;
    }
    sta = piccAntiA(SEL1, 0x01, UID);
    if(Ok == sta)
    {
        DrvPutStr("UID:");
        DrvPrintBuff(UID,4);
    }
    else
    {
        sky1311Reset();
        DrvPutStr("Anti error!\r\n");
        return 2;
    }
    sta = piccSelectA(SEL1,UID, tmpBuf);
    if(Ok == sta)
    {
        DrvPrintf("SAK:0x%X",tmpBuf[0]);
    }
    else
    {
        sky1311Reset();
        DrvPutStr("Select error!\r\n");
        return 3;
    }
    for(sector_num=0;sector_num<4;sector_num++)
    {
        sta = M1_Authentication(M1_AUTH_KEYA, default_key, UID, sector_num*4);
        if(Ok == sta)
        {
            for(block_num=0;block_num<4;block_num++)
            {
                sta = M1_Read(block_num+sector_num*4, mifare_block_buff);
                DrvPutStr("Block ");
                DrvPutStr(" : ");
            }
        }
        else{
            DrvPutStr("Sector 0x");
            DrvPutStr("Authentication Error\r\n");
        }
        uart_newrow();

        sky1311WriteReg(ADDR_M1_CTRL, 0x00);
        piccHaltA();
        DelayMS(5);
        sta = piccRequestA(tmpBuf);
        if(Ok == sta)
        {
            sta = piccSelectA(SEL1,UID, tmpBuf);
            if(Ok != sta){
                DrvPutStr("PICC Select Error!\r\n");
            }
        }
        else
        {
            DrvPutStr("PICC Request Error!\r\n");
        }
    }
    sky1311Reset();
    LED_OFF();//GPIO_ResetBits(GPIOD, 1);   //LED off
    return 0;
}
#endif
#if 0
/***************************************************************
    CPU卡操作示例
   读取CPU卡的UID等，并使用部分APDU做测试
   寻卡等操作时，显示操作结果
   有错误时显示错误信息
****************************************************************/
BYTE SmartTypeA_readDemo(PSky1311t pSky)
{
    sta_result_t sta;
    BYTE UID[10];
    BYTE tmpBuf[100];
    WORD tmpSize;
    SKY1311_ENABLE(); 	    // chip enable PD2
    DelayMS(1);
    sky1311Init();			// init sky1311s
    resetPicc();            // 复位卡片
    typeAOperate();
    sta = piccRequestA(tmpBuf);
    if(Ok == sta)
    {
        DrvPutStr("\r\nType-A Card test\r\n");
        DrvPrintf("ATQA:0x%X\r\n",tmpBuf[0]);
    }
    else
    {
        sky1311Reset();
        return 1;
    }
    sta = piccAntiA(SEL1, 0x01, UID);
    if(Ok == sta)
    {

        DrvPutStr("UID:");
        DrvPrintBuff(UID,4);
    }
    else
    {
        sky1311Reset();
        DrvPutStr("Anti error!");
        return 2;
    }
    sta = piccSelectA(SEL1,UID, tmpBuf);
    if(Ok == sta)
    {
        DrvPrintf("SAK:0x%X\r\n",tmpBuf[0]);
    }
    else
    {
        sky1311Reset();
        uart_puts("Select error!");
        return 3;
    }
    sta = piccATS(tmpBuf);
    if(sta == NoResponse){
        sky1311Reset();
        uart_puts("RATS error!");
        return 4;
    }
    else{

    	uart_puts("RATS : ");
    	uart_printBuffer(tmpBuf,32);
        uart_newrow();
    }

     /* select MF file */
    tmpBuf[0] = 0x02;
    tmpBuf[1] = 0x00;
    tmpBuf[2] = 0xA4;   // command: select file
    tmpBuf[3] = 0x00;
    tmpBuf[4] = 0x00;
    tmpBuf[5] = 0x02;
    tmpBuf[6] = 0x3F;
    tmpBuf[7] = 0x00;   // MF
    sky1311_fifo_tx(TYPE_A, tmpBuf, 8);
    sta = sky1311_fifo_rx(TYPE_A, RATE_OFF, tmpBuf, &tmpSize);

    if(sta == NoResponse)
    	return NoResponse;
    else{
        DrvPutStr("Select MF:");
    	DrvPrintBuff(tmpBuf,tmpSize-2);
    }
#if 0
    /*获取随机数*/
    tmpBuf[0] = 0x02;
    tmpBuf[1] = 0x00;
    tmpBuf[2] = 0x84;
    tmpBuf[3] = 0x00;
    tmpBuf[4] = 0x00;
    tmpBuf[5] = 0x08;
    sky1311_fifo_tx(TYPE_A, tmpBuf, 6);
    sta = sky1311_fifo_rx(TYPE_A, RATE_OFF, tmpBuf, &tmpSize);
    if(sta == NoResponse)
    	return NoResponse;
    else{
        DrvPutStr("RM:");
    	DrvPrintBuff(tmpBuf,tmpSize-2);
    }
#endif
    sky1311Reset();
    return 0;
}
#endif

/***************************************************************
    M1卡读取前5个扇区操作示例
   读取扇区数据并通过串口打印出来
   寻卡等操作时，显示操作结果
   有错误时显示错误信息
****************************************************************/
#ifdef SUPPORT_M1
extern u8 m1Flg;
extern u8 m1Secret[];
extern u8 m1BankData[];
extern u8 m1Bank;
sta_result_t mifare1_test(PSky1311t pSky,BYTE *UID)
{
	sta_result_t sta;
	BYTE tmpBuf[20];
	//u8 block_num = 4;
    //u8 keyb[]={1,2,3,4,5,6};
	//BYTE sector_num, block_num;
	if(m1Flg == M1_WRITE_CARD)
	{
		DrvPutStr("\r\nWrite M1 test\r\n");
		sta = M1_Authentication_Auto(pSky,M1_AUTH_KEYA, m1Secret, UID, m1Bank);
		if(sta_Ok == sta)
		{
			DrvPutStr("\r\nWrite M1 secret effi SUCC!\r\n");
			sky1311ReadReg(pSky,ADDR_M1_CTRL);
			//u8 writeData[] = "1234567890222222";
			sta = M1_Write(pSky,m1Bank, m1BankData);
			//sky1311WriteReg(pSky,ADDR_M1_CTRL, 0x00);
			if(sta_Ok == sta)
			{
				DrvPutStr("Block write SUCC!\r\n");;
			}
			else
			{
				DrvPutStr("Block write FAIL!\r\n");
			}
			return sta;
		}
		else
		{
			DrvPutStr("\r\nWrite M1 effi FAIL\r\n");
			piccHaltA(pSky);
			sky1311WriteReg(pSky,ADDR_M1_CTRL, 0x00);
			return sta_Error;
		}
	}
	else
	{
		DrvPutStr("\r\nRead M1 test\r\n");
		sta = M1_Authentication_Auto(pSky,M1_AUTH_KEYA, m1Secret, UID, m1Bank);
		if(sta_Ok == sta)
		{
			DrvPutStr("\r\nRead M1 secret effi SUCC!\r\n");
			sta = M1_Read(pSky,m1Bank, tmpBuf);
			DrvPutStr("Block ");
			DrvPutStr(" : ");
			DrvPrintBuff(tmpBuf,16);
			memcpyStr(m1BankData,tmpBuf,16);
			sky1311WriteReg(pSky,ADDR_M1_CTRL, 0x81);
			//sta = sta_Ok;
			if(sta_Ok == sta)
			{
				DrvPutStr("Read M1 bank data SUCC!\r\n");;
			}
			else
			{
				DrvPutStr("Read M1 bank data FAIL!\r\n");
			}
			return sta;
		}
		else
		{
			DrvPutStr("\r\nnRead M1 secret effi FAIL!\r\n");
			piccHaltA(pSky);
			sky1311WriteCmd(pSky,CMD_SW_RST);
			McuDelayMs(2);
			sky1311WriteReg(pSky,ADDR_TIME_OUT2, 0x8F);
			sky1311WriteReg(pSky,ADDR_TIME_OUT1, 0xFF);
			sky1311WriteReg(pSky,ADDR_TIME_OUT0, 0xFF);
			sky1311WriteReg(pSky,ADDR_RX_PUL_DETA, 0x34);
			sky1311WriteReg(pSky,ADDR_RX_PRE_PROC, 0x00);
			sky1311WriteReg(pSky,ADDR_IRQ_EN, IRQ_M1_EN|IRQ_TOUT_EN|IRQ_TX_EN|IRQ_RX_EN|IRQ_HIGH_EN|IRQ_LOW_EN);
			sta = piccWakeupA(pSky,tmpBuf);
            if(sta != sta_Ok)
            {
				return sta_Error;
			}
            //    return sta;
            sta = piccSelectA(pSky,SEL1,UID,tmpBuf);
			return sta_Error;
		}
		/*sky1311WriteReg(pSky,ADDR_M1_CTRL, 0x00);
		//piccHaltA(pSky);
		//pSky->pConst->DelayMs(3);
		//piccRequestA(pSky,tmpBuf);
		//if(sta_Ok == sta)
		{
			piccSelectA(pSky,SEL1,UID, tmpBuf);
		}
		else
		{
			DrvPutStr("PICC Request Error!\r\n");
		}
		return sta;
		*/
	}

}

#else
sta_result_t mifare1_test(PSky1311t pSky,BYTE *UID)
{
    sta_result_t sta;
    BYTE tmpBuf[20];
    BYTE default_key[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    BYTE sector_num, block_num;
    //DrvPutStr("\r\nRead Mifare 1 test\r\n");
    for(sector_num=0;sector_num<4;sector_num++)
    {
        sta = M1_Authentication_Auto(pSky,M1_AUTH_KEYA, default_key, UID, sector_num*4);
        if(sta_Ok == sta)
        {
            for(block_num=0;block_num<4;block_num++)
            {
                sta = M1_Read(pSky,block_num+sector_num*4, tmpBuf);
                //DrvPutStr("Block ");
                //DrvPutStr(" : ");
                //DrvPrintBuff(tmpBuf,16);
            }
        }
        else{
            //DrvPutStr("Sector 0x");
            //DrvPutStr("Authentication Error\r\n");
        }
        sky1311WriteReg(pSky,ADDR_M1_CTRL, 0x00);
        piccHaltA(pSky);
        pSky->pConst->DelayMs(3);
        sta = piccRequestA(pSky,tmpBuf);
        if(sta_Ok == sta)
        {
            sta = piccSelectA(pSky,SEL1,UID, tmpBuf);
            if(sta_Ok != sta){
                //DrvPutStr("PICC Select Error!\r\n");
            }
        }
        else
        {
            //DrvPutStr("PICC Request Error!\r\n");
        }
    }
#if 0
    /*** 测试写入 ***/
    tmpBuf[0] = 'A';
    tmpBuf[1] = 'B';
    tmpBuf[2] = 'C';
    sector_num = 2;
    sta = M1_Authentication(M1_AUTH_KEYA, default_key, UID, sector_num*4);
    if(Ok == sta)
    {
        for(block_num=0;block_num<3;block_num++)
        {
            sta = M1_Write(block_num+sector_num*4, tmpBuf);
            if(sta != Ok)
                break;
        }
    }
#endif
    /*** 测试写入结束 ***/
    sky1311Reset(pSky);
    return sta_Ok;
}
#endif
/***************************************************************
    CPU卡操作示例
   读取CPU卡的UID等，并使用部分APDU做测试
   寻卡等操作时，显示操作结果
   有错误时显示错误信息
****************************************************************/
sta_result_t smartTypeA_test(PSky1311t pSky,PTagInfo pTag)
{
    sta_result_t sta;
    BYTE tmpBuf[100];
    //DrvPutStr("\r\nSmart Type A test\r\n");
    sta = piccATS(pSky,tmpBuf);
    if(sta == sta_NoResponse){
        sky1311Reset(pSky);
        //DrvPutStr("\r\nRATS error!\r\n");
        return sta_ErrorAts;
    }
    else{
    	//DrvPutStr("RATS : ");
    	//DrvPrintBuff(tmpBuf,32);
    }
#if 0
    /* 00 a4 04 00 00 */
    WORD tmpSize;
    tmpBuf[0] = 0x02;
    tmpBuf[1] = 0x00;
    tmpBuf[2] = 0xA4;   // command: select file
    tmpBuf[3] = 0x04;
    tmpBuf[4] = 0x00;
    tmpBuf[5] = 0x00;
    sky1311_fifo_tx(TYPE_A, tmpBuf, 6);
    sta = sky1311_fifo_rx(TYPE_A, RATE_OFF, tmpBuf, &tmpSize);

    if(sta == NoResponse)
    {
        sky1311Reset();
    	return NoResponse;
    }
    else{
        DrvPutStr("Select SD:");
    	DrvPrintBuff(tmpBuf,tmpSize-2);
    }

     /* select MF file */
    tmpBuf[0] = 0x02;
    tmpBuf[1] = 0x00;
    tmpBuf[2] = 0xA4;   // command: select file
    tmpBuf[3] = 0x00;
    tmpBuf[4] = 0x00;
    tmpBuf[5] = 0x02;
    tmpBuf[6] = 0x3F;
    tmpBuf[7] = 0x00;   // MF
    sky1311_fifo_tx(pSky,TYPE_A, tmpBuf, 8);
    sta = sky1311_fifo_rx(pSky,TYPE_A, RATE_OFF, tmpBuf, &tmpSize);

    if(sta == NoResponse)
    {
        sky1311Reset();
    	return NoResponse;
    }
    else{
        DrvPutStr("Select MF:");
    	DrvPrintBuff(tmpBuf,tmpSize-2);
    }
#if 0
    /*获取随机数*/
    tmpBuf[0] = 0x02;
    tmpBuf[1] = 0x00;
    tmpBuf[2] = 0x84;
    tmpBuf[3] = 0x00;
    tmpBuf[4] = 0x00;
    tmpBuf[5] = 0x08;
    sky1311_fifo_tx(pSky,TYPE_A, tmpBuf, 6);
    sta = sky1311_fifo_rx(pSky,TYPE_A, RATE_OFF, tmpBuf, &tmpSize);
    if(sta == NoResponse)
    {
        sky1311Reset(pSky);
    	return NoResponse;
    }
    else{
        DrvPutStr("RM:");
    	DrvPrintBuff(tmpBuf,tmpSize-2);
    }
#endif

#if 0
    /* 读一卡通有关的信息 */
    tmpBuf[0] = 0x03;
    tmpBuf[1] = 0x00;
    tmpBuf[2] = 0xA4;
    tmpBuf[3] = 0x00;
    tmpBuf[4] = 0x00;
    tmpBuf[5] = 0x02;
    tmpBuf[6] = 0x10;
    tmpBuf[7] = 0x01;
    sky1311_fifo_tx(pSky,TYPE_A, tmpBuf, 8);
    sta = sky1311_fifo_rx(pSky,TYPE_A, RATE_OFF, tmpBuf, &tmpSize);
    if(sta == NoResponse)
    {
        sky1311Reset(pSky);
    	return NoResponse;
    }
    else{
        DrvPutStr("Read Card Info:");
    	DrvPrintBuff(tmpBuf,tmpSize-2);
    }
    /* 读余额 */
    tmpBuf[0] = 0x02;
    tmpBuf[1] = 0x80;
    tmpBuf[2] = 0x5C;
    tmpBuf[3] = 0x00;
    tmpBuf[4] = 0x02;
    tmpBuf[5] = 0x04;
    sky1311_fifo_tx(pSky,TYPE_A, tmpBuf, 6);
    sta = sky1311_fifo_rx(pSky,TYPE_A, RATE_OFF, tmpBuf, &tmpSize);
    if(sta == NoResponse)
    {
        sky1311Reset(pSky);
    	return NoResponse;
    }
    else{
        DrvPutStr("Read Balance:");
    	DrvPrintBuff(tmpBuf,tmpSize-2);
#endif
    }

    for(i=1;i<=10;i++)
    {
        /* 读取交易记录 */
        tmpBuf[0] = 0x02;
        tmpBuf[1] = 0x00;   // CLA Class
        tmpBuf[2] = 0xB2;   // INS Instruction
        tmpBuf[3] = i;//0x01;   // P1 Parameter 1
        tmpBuf[4] = 0xC4;//0xC5;   // P2 Parameter 2
        tmpBuf[5] = 0x00;   // Le
        sky1311_fifo_tx(pSky,TYPE_A, tmpBuf, 6);
        sta = sky1311_fifo_rx(pSky,TYPE_A, RATE_OFF, tmpBuf, &tmpSize);
        if(sta == NoResponse)
        {
            return NoResponse;
        }
        else{
            DrvPutStr("Read Record:");
            DrvPrintBuff(tmpBuf,tmpSize-2);
        }
    }
#endif //if 0
    sky1311Reset(pSky);
    return sta_Ok;
}

//u8 cardBuff[30];
//extern BTType gBT;
extern u8 deviceFlg;
u8 tempTest[30];
extern unsigned char ledFlg;

void cardTest(PSky1311t pSky)
{
	u8 i=0,j=0,h=0;
	for(i=0;i<30;i++)
	{
		tempTest[i] = sky1311ReadReg(pSky,i);
		if(tempTest[i] == 0)
		{
			j ++;
		}
		else if(tempTest[i] == 0xff)
		{
			h ++;
		}
		printf("%x ",tempTest[i]);
	}
	printf("\n");

	
}

/******************************************************************************/
/*  Type A PICC test */
extern u8 m1Return;
u8 cartTypeFlg=0;
u8 sky1311ReadFlg=1;
extern u8 sky1311ReadBuff[0x3f];

#ifdef SUPPORT_CARD_EMULATION
sta_result_t TypeA_test(PSky1311t pSky,PTagInfo pTag)
{
	sta_result_t sta;
	BYTE UID[20];
	BYTE tmpBuf[60];
	BYTE i=10;
	u16 tmpSize;
	//u8 cartTypeTemp=0;

	while(i--)
	{
		pSky->pConst->EnablePdPin();//SKY1311_ENABLE(); 		// chip enable PD2
		pSky->pConst->DelayMs(1);  
		sky1311Init(pSky);			// init sky1311s
		pSky->pConst->DelayMs(10);	
		resetPicc(pSky);
		typeAOperate(pSky);   
		sta = piccWakeupA(pSky,tmpBuf); 	   // send WUPA command to card
		if(sta_Ok == sta)
		{
			pTag->uATQA = tmpBuf[0];
		}
		else
		{
			//PutStr("piccWakeupA FAIL\r\n");
			sky1311Reset(pSky);
			return sta_ErrorRequest;			// 这里返回，有可能是没卡或者没读到
		}
		Printf("i=%d\r\n", i);
		sta = piccAntiA(pSky,SEL1, 0x01, UID);	 // 防碰撞循环，测试获得一张卡的UID，参数0x0表示遇到冲突时选择1
		if(sta_Ok == sta)
		{
			 BYTE uXor=0;
			 for(u8 i=0;i<4;i++)
			 {
				 uXor ^= UID[i];
			 }
			if(uXor == UID[4])
			{
				 pTag->TagType=TAG_TYPE_14443A0;
				 memcpy(pTag->uCardId,UID,4);
				 pTag->uCardIdLen = 4;
				 PutStr("xor SUCC\r\n");
			}
		}
		else
		{
			PutStr("piccAntiA FAIL\r\n");
			sky1311Reset(pSky);
			continue;
			//return sta_ErrorAnticollision;
		}
		memset(tmpBuf,0,sizeof(tmpBuf));
		sta = piccSelectA(pSky,SEL1,UID, tmpBuf);
		if((tmpBuf[0]==0x28)||(tmpBuf[0]==0x08))
		{
			cartTypeFlg = tmpBuf[0];
			PutStr("huawei card type\r\n");
			pTag->TagType=TAG_TYPE_14443A0;
			sky1311Reset(pSky);
			return sta_Ok;
		}
		else if(tmpBuf[0]==0x20) //手机CARD EMULATION
		{
			cartTypeFlg = tmpBuf[0];
			PutStr("card emulate type\r\n");
		}
		else 
		{
			PutStr("card  unknown type\r\n");
			sky1311Reset(pSky);
			return sta_M1Error;
		}
		if(sta_Ok == sta)
		{
			//PutStr("piccSelectA OK\r\n");
		}
		else
		{
			PutStr("piccSelectA FAIL\r\n");
			sky1311Reset(pSky);
			continue;
			//return sta_ErrorSelect;
		}
		sta = piccATS(pSky,tmpBuf);
		if(sta == sta_NoResponse){
			PutStr("piccATS FAIL\r\n");
			sky1311Reset(pSky);
			continue;
			//return sta_RxParity;
		}
		else{
			PutStr("piccATS OK\r\n");
		}
		tmpBuf[0] = 0x02;//包头
		tmpBuf[1] = 0x00;
		tmpBuf[2] = 0xA4;
		tmpBuf[3] = 0x04;
		tmpBuf[4] = 0x00;
		tmpBuf[5] = 0x05;
		tmpBuf[6] = 0xF2;
		tmpBuf[7] = 0x22;
		tmpBuf[8] = 0x22;
		tmpBuf[9] = 0x22;
		tmpBuf[10] = 0x22;
		tmpSize = 0;
		sky1311_fifo_tx(pSky,TYPE_A, tmpBuf, 11);
		sta = sky1311_fifo_rx(pSky,TYPE_A, RATE_OFF, tmpBuf, &tmpSize);
		if((sta != sta_NoResponse)&&(tmpSize>5))
		{
			break;
		}
		if(sta == sta_NoResponse)
		{
			PutStr("sky1311_fifo_rx fail\r\n");
		}
		else
		{
			if(tmpSize<=5)
			{
				PutStr("tmpSize <= 5\r\n");
			}
			PutStr("read_data:");
			PrintBuff(tmpBuf,tmpSize);
			PutStr("\r\n");
		}
	}
	if(sta == sta_NoResponse)
	{
		sky1311Reset(pSky);
		return sta_NoResponse;
	}	
	else
	{
	  if(tmpSize>5)
	  {
		pTag->TagType=TAG_TYPE_14443A0;
		memcpy(pTag->uCardId,&tmpBuf[1],tmpSize-5);
		pTag->uCardIdLen = tmpSize-5;
		PutStr("succ card_emulate_data:");
		PrintBuff(tmpBuf,tmpSize);
		PutStr("\r\n");
	  }
	  else
	  {
		PutStr("fail card_emulate_data:");
		PrintBuff(tmpBuf,tmpSize);
		PutStr("\r\n");
		sky1311Reset(pSky);
		return sta_M1Error;
	  }
	}
	sky1311Reset(pSky);
	return sta_Ok;
}

#else
sta_result_t TypeA_test(PSky1311t pSky,PTagInfo pTag)
{
	sta_result_t sta;
	BYTE UID[20];
	BYTE tmpBuf[60];
	#ifdef SUPPORT_M1
	BYTE i;
	#elif defined SUPPORT_CARD_EMULATION
	BYTE i;
	#endif
    /*u8 SAK=0;
    u8 *uid;
    u8 sel=SEL1;
    u8 uidSize=0;
	*/
	int testi=0,j=0;
	pSky->pConst->EnablePdPin();//SKY1311_ENABLE(); 		// chip enable PD2
	#ifdef SUPPORT_SKY1311_TEST	
	if(sky1311ReadFlg)
	{
		pSky->pConst->DelayMs(2);  
		//sky1311WriteReg(pSky,ADDR_M1_CTRL, 0x02);			  // crcB enable
		//i = sky1311ReadReg(pSky,ADDR_M1_CTRL);			  // crcB enable
		//Printf("ADDR_M1_CTRL0=%d\r\n",i);
		sky1311ReadAll(pSky);
		printf("read sky1311 reg0:\r\n");
		//PrintBuff(sky1311ReadBuff,0x40);
		for(testi=0;testi<0x40;testi++){
			printf("%x ",sky1311ReadBuff[testi]);
		}
		printf("\r\n");
	}
	#endif
	sky1311Init(pSky);			// init sky1311s
	#ifdef SUPPORT_SKY1311_TEST
	if(sky1311ReadFlg)
	{
		//sky1311WriteReg(pSky,ADDR_M1_CTRL, 0x30);			  // crcB enable
		//i = sky1311ReadReg(pSky,ADDR_M1_CTRL);			  // crcB enable
		//Printf("ADDR_M1_CTRL1=%d\r\n",i);
		sky1311ReadAll(pSky);
		sky1311ReadFlg = 0;
		printf("read sky1311 reg1:\r\n");
		//PrintBuff(sky1311ReadBuff,0x40);
		for(testi=0;testi<0x40;testi++){
			printf("%x ",sky1311ReadBuff[testi]);
		}
		printf("\r\n");

	}
	#endif

	pSky->pConst->DelayMs(10);  
	#ifdef SUPPORT_CARD_EMULATION
	resetPicc(pSky);
	#endif
	typeAOperate(pSky);
    //sky1311WriteReg(pSky,ADDR_MFOUT_SEL, 0X44);
	sta = piccWakeupA(pSky,tmpBuf); 	   // send WUPA command to card
	if(sta_Ok == sta)
	{
		pTag->uATQA = tmpBuf[0];
	}
	else
	{
		sky1311Reset(pSky);
		return sta_ErrorRequest;			// 这里返回，有可能是没卡或者没读到
	}
 //   uid = &UID[0];
//    sel = SEL1;
	sta = piccAntiA(pSky,SEL1, 0x01, UID);	 // 防碰撞循环，测试获得一张卡的UID，参数0x0表示遇到冲突时选择1
	if(sta_Ok == sta)
	{
		#ifdef SUPPORT_ATS_TYPEA_CPU_CARD
		PutStr("TypeA_test\r\n");
		#elif defined SUPPORT_CARD_EMULATION
		PutStr("TypeA_test\r\n");
		#elif defined SUPPORT_M1
		 BYTE uXor=0;
		 for(i=0;i<4;i++)
		 {
			 uXor ^= UID[i];
		 }
		if(uXor == UID[4])
		{
	        pTag->TagType=TAG_TYPE_14443A0;
			memcpy(pTag->uCardId,UID,4);
			pTag->uCardIdLen = 4;
			if(m1Flg == 0)
			{
				sky1311Reset(pSky);
				return sta_Ok;
			}
			//PutStr("\r\nUID:");
			//PrintBuff(UID,4);
			//PutStr("\r\n");
			//Printf("Freq=%dMHz\r\n", SystemCoreClock/1000000);
		}
		else
		{
			PutStr("\r\nAnti xor error!\r\n");
			sky1311Reset(pSky);
			return sta_ErrorAnticollision;
		}
		#else
		BYTE uXor=0;
		for(u8 i=0;i<4;i++)
		{
			uXor ^= UID[i];
		}
       if(uXor == UID[4])
       {
	        pTag->TagType=TAG_TYPE_14443A0;
			memcpy(pTag->uCardId,UID,4);
			pTag->uCardIdLen = 4;
			
			
			sky1311Reset(pSky);
			return sta_Ok;
       }
       else
	   {
	        sky1311Reset(pSky);
			return sta_ErrorAnticollision;
       }
	   #endif
	}
	else
	{
		sky1311Reset(pSky);
		return sta_ErrorAnticollision;
	}

	#ifdef SUPPORT_M1
	if(m1Flg)
	{
		m1Return = 0;
		/*************下面是选卡以及对卡的操作，若只读UID的话到此结束***********/
		sta = piccSelectA(pSky,SEL1,UID, tmpBuf);//pier
		if(sta_Ok == sta)
		{
			PutStr("\r\npiccSelectA OK!\r\n");						
			//SAK = tmpBuf[0];
			pTag->uSAK = tmpBuf[0];
			sta = mifare1_test(pSky,UID);
			m1Return = 1;
		}
		else
		{
			PutStr("\r\npiccSelectA error!\r\n");			
		}
	}
	sky1311Reset(pSky);
	return sta; 	
	#elif defined SUPPORT_ATS_TYPEA_CPU_CARD
    u16 tmpSize;
    memset(tmpBuf,0,sizeof(tmpBuf));
	sta = piccSelectA(pSky,SEL1,UID, tmpBuf);
	if(sta_Ok == sta)
	{
		//SAK = tmpBuf[0];
		//pTag->uSAK = tmpBuf[0];
	}
	else
	{
		sky1311Reset(pSky);
		return sta_ErrorSelect;
	}
    sta = piccATS(pSky,tmpBuf);
    if(sta == sta_NoResponse){
        sky1311Reset(pSky);
        //uart_puts("RATS error!");
        return sta_RxParity;
    }
    else{
    }
    tmpBuf[0] = 0x02;//包头
    tmpBuf[1] = 0x00;
    tmpBuf[2] = 0xA4;
    tmpBuf[3] = 0x00;
    tmpBuf[4] = 0x00;
    tmpBuf[5] = 0x02;
    tmpBuf[6] = 0x10;
    tmpBuf[7] = 0x01;//*/  
    sky1311_fifo_tx(pSky,TYPE_A, tmpBuf, 8);
    sta = sky1311_fifo_rx(pSky,TYPE_A, RATE_OFF, tmpBuf, &tmpSize);

    if(sta == sta_NoResponse)
    	return sta_NoResponse;
    else{
        //uart_puts("Select MF:");
    	//uart_printBuffer(tmpBuf,tmpSize-2);
       // uart_newrow();
    }
    tmpBuf[0] = 0x03;//0x02;//包头
    tmpBuf[1] = 0x00;
    tmpBuf[2] = 0xB0;
    tmpBuf[3] = 0x95;
    tmpBuf[4] = 0x00;
    tmpBuf[5] = 0x21;
    sky1311_fifo_tx(pSky,TYPE_A, tmpBuf, 6);
    sta = sky1311_fifo_rx(pSky,TYPE_A, RATE_OFF, tmpBuf, &tmpSize);
    if(sta == sta_NoResponse)
    	return sta_NoResponse;
    else{
        //uart_puts("RM:");
    	//uart_printBuffer(tmpBuf,tmpSize-2);
        //uart_newrow();
	  pTag->TagType=TAG_TYPE_14443A0;
	  memcpy(pTag->uCardId,&tmpBuf[13],8);
	  pTag->uCardIdLen = 8;
    }
	
	/*u32 uUid;
	uUid = (UID[0]<<24)|(UID[1]<<16)|(UID[2]<<8)|(UID[3]);
	
	Printf("读卡UUID=%x\r\n",uUid);
	*/
	sky1311Reset(pSky);
	return sta_Ok;
	#endif
}
#endif

/*
{
    sta_result_t sta;
    BYTE SAK=0;
    //u8 ATQA[2]={0};
    BYTE UID[12];
    BYTE tmpBuf[10];
    u8 uidSize=0;
    u8 *uid;
    u8 sel=SEL1;
    u8 i;

    pSky->pConst->EnablePdPin();//SKY1311_ENABLE(); 	    // chip enable PD2
    sky1311Init(pSky);			// init sky1311s
    //sky1311WriteReg(pSky,ADDR_ANA_CFG3, 0x81);
    pSky->pConst->DelayMs(10);  //Crystal Stable time    
    typeAOperate(pSky);
    sta = piccWakeupA(pSky,tmpBuf);        // send WUPA command to card
    if(sta_Ok == sta)
    {
        //ATQA[0] = tmpBuf[1];
        //ATQA[1] = tmpBuf[0];
        uidSize = tmpBuf[0]>>6;
        pTag->uATQA = tmpBuf[0];
    }
    else
    {
        sky1311Reset(pSky);
        return sta_ErrorRequest;            // 这里返回，有可能是没卡或者没读到
    }
    uid = &UID[0];
    sel = SEL1;
	do{
		sta = piccAntiA(pSky,sel, 0x01, uid);	 // 防碰撞循环，测试获得一张卡的UID，参数0x0表示遇到冲突时选择1
		if(sta_Ok == sta)
		{
            u8 checkSum = uid[0] ^ uid[1] ^ uid[2] ^ uid[3];
			if(checkSum != uid[4])
			{
				#if DEBUG==1
				uart_puts("\r\nAnti error!\r\n");
				#endif
				sky1311Reset(pSky);
				return sta_ErrorAnticollision;
			}
			
		}
		else
		{
            sky1311Reset(pSky);
			return sta_ErrorAnticollision;
		}
		sta = piccSelectA(pSky,sel,uid, tmpBuf); 	   // Select PICC
		if(sta_Ok == sta)
		{
            SAK = tmpBuf[0];
		}
		else
		{
			return sta_ErrorSelect;
		}
        if(uidSize!=0 && uid[0] == 0x88)
        {
            for(i=0;i<3;i++){
                uid[i] = uid[i+1];
            }
            uid += 3;
            sel += 2;

        }
		
	}while( (SAK&0x04)!=0 );
    pTag->TagType=TAG_TYPE_14443A0;
    memcpy(pTag->uCardId,UID,4);
    pTag->uCardIdLen = 4;

 
    sky1311Reset(pSky);
    return sta;
}
*/
/***************************************************************
    TYPE B卡操作示例
   读取TYPE B卡的UID等，并使用部分APDU做测试
   寻卡等操作时，显示操作结果
   有错误时显示错误信息
****************************************************************/
u8  uStatus=0;
extern BYTE getData[64];

//extern CommunicateType gBle;


void bleGetRealDataPro()
{
	//gBle.uDataValidLen -= 22;
	//memcpyStr(gBle.btUartData,&gBle.btUartData[21],gBle.uDataValidLen);

}
/*
void btGet(void)
{
	//u8 i=0,uLength=0;
	while(1)
	{
		if(btGetHead())
		{
			bleGetRuoChanPro();
			uStatus = gBle.btUartData[gBle.uDataValidLen-1];
			bleGetRealDataPro();

			return;
		}
	}
}*/

extern u8 bleFlg;
/*void btSend1(u8 *mac,u8 *data,u8 length)
{
	u8 i=0;

	length += 9;
	uartDataTemp[8+length] = 0;// 10	18
	for(i=0;i<(length-9);i++)
	{
		uartDataTemp[17+i] = *(data+i);
		uartDataTemp[8+length] += *(data+i);
	}	
	uartDataTemp[0] = CMD_TYPE1;
	uartDataTemp[1] = CMD_TYPE2;
	uartDataTemp[2] = CMD_TYPE3;
	uartDataTemp[3] = CMD_TYPE4;
	uartDataTemp[4] = CMD_LE_XFER2H >> 8;
	uartDataTemp[5] = CMD_LE_XFER2H;
	uartDataTemp[6] = length >> 8;
	uartDataTemp[7] = length;
	for(i=0;i<9;i++)
	{
		uartDataTemp[8+i] = *(mac+i);
	}
	for(i=0;i<9;i++)
	{
		uartDataTemp[8+length] += *(mac+i);
	}	
	uartDataTemp[8+length] = 256-uartDataTemp[8+length];

	for(i=0;i<length+9;i++)
	{
		//McuBtPutChar(uartDataTemp[i]);
	}
}*/
extern u8 xOrCnt;
extern u8 btMac[9];
void btSendPack1(u8 *uData,u8 uLength)
{

}
//u8 uartDataLengthContinut=0;
u8 uartData[200];
//u16 uartDataTemp1Length=0;
//u8 uartDataXor=0;

u8 btGetHead1(void)
{

	return 0;
}
u16 idCardTimeOutBuff=0;

void btGet(void)
{

}
	
void idCardTimeOutPro()
{
	if(idCardTimeOutBuff)
	{
		idCardTimeOutBuff --;
	}
}
u8 testTimers=0;
sta_result_t SmartTypeB_test(PSky1311t pSky,PTagInfo pTag)
{
    sta_result_t sta;
	
    nAQTB_t ATQB;
    BYTE tmpBuf[255];//100
    WORD tmpSize;
    BYTE  attSize;
	BYTE testBuff[8];

    pSky->pConst->EnablePdPin();//SKY1311_ENABLE(); 	    // chip enable PD2
    sky1311Init(pSky);			// init sky1311s
    pSky->pConst->DelayMs(10);  //Crystal Stable time
    typeBOperate(pSky);

    sta = piccRequestB(pSky,REQALL,0, 0, &ATQB);
    if(sta_Ok != sta)
    {
        sky1311Reset(pSky);
        return sta_NoResponse;
    }
    //PutStr("\r\nType-B Card test\r\n");
    sta = piccAttrib(pSky,ATQB.PUPI, 0, 8, 0, 1, tmpBuf, 0, tmpBuf, &attSize);
    if(sta_Ok != sta)
    {
        sky1311Reset(pSky);
        return sta_NoResponse;
    }
	testTimers = 0;
    /* 读身份证ID */
    tmpBuf[0] = 0x00;
    tmpBuf[1] = 0x36;
    tmpBuf[2] = 0x00;
    tmpBuf[3] = 0x00;
    tmpBuf[4] = 0x08;
    //sky1311_fifo_tx(pSky,TYPE_B, tmpBuf, 5);
    //sta = sky1311_fifo_rx(pSky,TYPE_B, RATE_OFF, tmpBuf, &tmpSize);
	sta = ExchangeData(pSky,TYPE_B,RATE_OFF,tmpBuf,5,tmpBuf,&tmpSize);
	if(sta == sta_NoResponse)
	{
		sky1311Reset(pSky);
		return sta_NoResponse;
	}
	else{
		//DrvPutStr("ID Card:");
		pTag->TagType = TAG_TYPE_14443B;
		memcpy(pTag->uCardId,tmpBuf,8);
		pTag->uCardIdLen = 8;
		//DrvPrintBuff(tmpBuf,tmpSize-2);
		memcpy(testBuff,tmpBuf,8);
		DrvPrintBuff(tmpBuf,8);
		//DrvPrintf("ID Card:%d",testBuff);
	}
	sky1311Reset(pSky);
	return sta_Ok;

	/*
    if(sta == sta_NoResponse)
    {
        sky1311Reset(pSky);
    	return sta_NoResponse;
    }
    else{
        DrvPutStr("ID Card:");
        pTag->TagType = TAG_TYPE_14443B;
        memcpy(pTag->uCardId,tmpBuf,8);
        pTag->uCardIdLen = 8;
        DrvPrintBuff(tmpBuf,tmpSize-2);
    }
    sky1311Reset(pSky);
    return sta_Ok;
	*/
}


