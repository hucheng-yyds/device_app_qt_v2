/******************************************************************************/
/*               (C) 斯凯瑞利（北京）科技有限公司(SKYRELAY)                   */
/*                                                                            */
/* 此代码是斯凯瑞利（北京）科技有限公司为支持客户编写的示例程序的一部分       */
/* 所有使用斯凯瑞利相关芯片的用户可以无偿使用该代码，但需保留本声明部分       */
/* 本代码仅为演示使用，不保证符合所有的应用要求，当有特定规范或需求时，       */
/* 使用者需自行确认代码是否符合要求，不合要求时自行做出相应的修改。           */
/* (V1.10)                                                                    */
/******************************************************************************/
/**
 ******************************************************************************
 ** 文 件 名： iso14443_3a.c
 **
 ** 文件简述： 符合ISO14443-3 type A的操作函数，常量和变量
 **
 ** 版本历史:
 ** 2015-10-05 V1.00  EH   第一个正式版本
 ** 2017-3-25  V.10  Release    更新代码和函数结构，重新发布
 **
 ******************************************************************************/
#include "iso14443.h"

/**
 ******************************************************************************
 ** \简  述  type A "Request" 操作,command = 26H
 **
 ** \参  数  none
 ** \返回值  sta_result_t 操作状态，Ok：成功，其它：失败
 ******************************************************************************/
sta_result_t piccRequestA(PSky1311t pSky,BYTE *ATQA)
{
    sta_result_t sta;
    WORD tmpSize;
    BYTE tmpBuf[1];
    tmpBuf[0] = REQA;       // 0x26
    sky1311WriteReg(pSky,ADDR_TX_CTRL, TX_POLE_HIGH|TX_PARITY_ODD);      // TX odd parity, no CRC
    sky1311WriteReg(pSky,ADDR_RX_CTRL, RX_PARITY_EN|RX_PARITY_ODD);      // RX odd parity, no CRC
    sky1311WriteReg(pSky,ADDR_TX_BIT_NUM, 0x07);
    sky1311WriteReg(pSky,ADDR_TX_BYTE_NUM, 0x01);

    sky1311_fifo_tx(pSky,TYPE_A, tmpBuf, 1);
    sta = sky1311_fifo_rx(pSky,TYPE_A, RATE_OFF, ATQA, &tmpSize);
    if(sta == sta_Ok && tmpSize == 2)
        return sta_Ok;
    else
        return sta_NoResponse;
}

/**
 ******************************************************************************
 ** \简  述  type A "Wake-Up" 操作,command = 52H
 **
 ** \参  数  none
 ** \返回值  sta_result_t 操作状态，Ok：成功，其它：失败
 ******************************************************************************/
sta_result_t piccWakeupA(PSky1311t pSky,BYTE *ATQA)
{
    sta_result_t sta;
    WORD tmpSize;
    BYTE tmpBuf[1];

    tmpBuf[0] = WUPA;       // 0x52
    sky1311WriteReg(pSky,ADDR_TX_CTRL, TX_POLE_HIGH|TX_PARITY_ODD);      // TX odd parity, no CRC
    sky1311WriteReg(pSky,ADDR_RX_CTRL, RX_PARITY_EN|RX_PARITY_ODD);      // RX odd parity, no CRC
    sky1311WriteReg(pSky,ADDR_TX_BIT_NUM, 0x07);
    sky1311WriteReg(pSky,ADDR_TX_BYTE_NUM, 0x01);
    //sta = data_tx_rx(1, TYPE_A, RATE_OFF);
    //sky1311_fifo_tx(pSky,TYPE_A, tmpBuf, 1);
	//sta = sky1311_fifo_rx(pSky,TYPE_A, RATE_OFF, ATQA, &tmpSize);
    sta = ExchangeData(pSky,TYPE_A,RATE_OFF,tmpBuf,1,ATQA,&tmpSize);
	
    //if(tmpSize != 2)
    //    return sta_Error;
    //else
    return sta;
}
/**
 ******************************************************************************
 ** \简  述  type A卡防冲突循环
 **
 ** \参  数  SEL,  (93h, 95h, 97h)
 **          BYTE randBit 位冲突时选择的数，0或1;
 **          BYTE* uid 获得的UID保存区域的首地址
 ** \返回值  return Ok on success, Error on error
 ******************************************************************************/
/*sta_result_t piccAntiA(PSky1311t pSky,BYTE SEL, BYTE rand_bit, BYTE *uid)
{
    BYTE NVB= 0x20;
    BYTE currValidBytes=0, currValidBits=0;
    BYTE recBytes, recBits;
    BYTE recUID[5]={ 0 };      // 接收到的UID数
    BYTE hasCollision = 0;
	BYTE tmpSize = 0;
	BYTE tmpBuf[12];
	BYTE g_rx_buff[12];
    //BYTE i=0;
    u8 recBit8;

    sky1311WriteReg(pSky,ADDR_RX_NUM_H, COLL_EN);        // anti-collision on
    sky1311WriteReg(pSky,ADDR_TX_CTRL, TX_POLE_HIGH|TX_PARITY_ODD);
    sky1311WriteReg(pSky,ADDR_RX_START_BIT_NUM,0);

    tmpBuf[0] = SEL;
    tmpBuf[1] = NVB;
    if(sta_Ok != bitCollisionTrans(pSky,tmpBuf, 2 , 8)){
        sky1311WriteReg(pSky,ADDR_RX_NUM_H, 0x00);               // disable anti-collision
        return sta_Error;
    }
    pSky->pConst->DelayMs(2);//delay(5);
    //delay(5);
    do{
        recBytes = sky1311ReadReg(pSky,ADDR_RX_NUM_L);
		recBit8 = sky1311ReadReg(pSky,ADDR_RX_NUM_H);
        recBits = ((recBit8 & 0xf0) >> 4);
        tmpSize = sky1311ReadReg(pSky,ADDR_FIFO_LEN);
        sky1311ReadFifo(pSky,g_rx_buff, tmpSize);
        if((sky1311ReadReg(pSky,ADDR_RX_NUM_H) & RX_FORBID) == 0)   // no collision happens
            hasCollision = 0;
        else
            hasCollision = 1;

        if(currValidBits == 0){         // 当前UID全部是完整的字节，按字节拼接
            for(BYTE i=0;i<tmpSize;i++){
                recUID[currValidBytes++] = g_rx_buff[i];
            }
            currValidBits = recBits;
        }
        else{                           // 当前UID最后一个字节不完整，第一个字节需要按位拼接
             if(recBytes == 0){              // 仅收到一个字节新数据（可能也不完整）
                recUID[currValidBytes-1] += g_rx_buff[0]<<currValidBits;    // 按位拼接第一个字节
                currValidBits += recBits;
                if(currValidBits==8){
                    currValidBytes++;
                    currValidBits = 0;
                }
            }
            else{                           // 收到多个字节数据
                recUID[currValidBytes-1] += g_rx_buff[0];//<<currValidBits;   // 按位拼接第一个字节
                for(BYTE i=1;i<tmpSize;i++){           // 复制剩余的字节
                    recUID[currValidBytes++] = g_rx_buff[i];
                }
                currValidBits = recBits;
            }
        }
        if(hasCollision){
          //pSky->pConst->DelayMs(1);//delay(5);
            if(currValidBits==0){
                uid[currValidBytes] = rand_bit;
                currValidBits = 1;
                NVB = 0x20+((currValidBytes)<<4) |  currValidBits;
                currValidBytes++;
            }
            else if(currValidBits == 7){
                recUID[currValidBytes-1] += rand_bit<<7;
                currValidBits = 0;
                NVB = 0x20+((currValidBytes)<<4);
            }
            else{       // currValidBits= 1 ... 6
                uid[currValidBytes-1] += rand_bit<<currValidBits;
                currValidBits++;
                NVB = 0x20+((currValidBytes-1)<<4) |  currValidBits;
            }
            tmpBuf[0] = SEL;
            tmpBuf[1] = NVB;
            for(u8 i=0;i<currValidBytes;i++){
                tmpBuf[2+i] = uid[i];
            }
            sky1311WriteReg(pSky,ADDR_RX_START_BIT_NUM,currValidBits);
            if(sta_Ok != bitCollisionTrans(pSky,tmpBuf, currValidBytes+2 , currValidBits)){
                sky1311WriteReg(pSky,ADDR_RX_NUM_H, 0x00);// disable anti-collision
                sky1311WriteReg(pSky,ADDR_RX_START_BIT_NUM,0);
                return sta_Error;
            }
        }

    }while(hasCollision);

    sky1311WriteReg(pSky,ADDR_RX_NUM_H, 0x00); // disable anti-collision
    sky1311WriteReg(pSky,ADDR_RX_START_BIT_NUM,0);
    //memcpy(uid,recUID,5);
    for(u8 i=0;i<5;i++)
        uid[i] = recUID[i];
    return sta_Ok;
}
*/


sta_result_t piccAntiA(PSky1311t pSky,u8 SEL, u8 rand_bit, u8 *uid)
{
	u8 NVB= 0x20;
	u8 currValidBytes=0, currValidBits=0;
	u8 recBytes, recBits;
	u8 hasCollision = 0;
	u8 tmpSize = 0;
	u8 tmpBuf[12];
	u8 g_rx_buff[12];
	u8 i;
	u8 recBit8;

	sky1311WriteReg(pSky,ADDR_RX_NUM_H, COLL_EN);		// anti-collision on
	sky1311WriteReg(pSky,ADDR_TX_CTRL, TX_POLE_HIGH|TX_PARITY_ODD);
	sky1311WriteReg(pSky,ADDR_RX_START_BIT_NUM,0);
	tmpBuf[0] = SEL;
	tmpBuf[1] = NVB;
	if(sta_Ok != bitCollisionTrans(pSky,tmpBuf, 2 , 8)){
		sky1311WriteReg(pSky,ADDR_RX_NUM_H, 0x00);				// disable anti-collision
		return sta_Error;
	}
	pSky->pConst->DelayMs(2);//delay(5);
	//delay(5);
	do{
		recBytes = sky1311ReadReg(pSky,ADDR_RX_NUM_L);
		recBit8 = sky1311ReadReg(pSky,ADDR_RX_NUM_H);
		recBits = (recBit8&0xf0)>>4;
		//recBits = ((sky1311ReadReg(ADDR_RX_NUM_H) & 0xf0) >> 4);
		tmpSize = sky1311ReadReg(pSky,ADDR_FIFO_LEN);
		sky1311ReadFifo(pSky,g_rx_buff, tmpSize);
		if((sky1311ReadReg(pSky,ADDR_RX_NUM_H) & RX_FORBID) == 0)	// no collision happens
			hasCollision = 0;
		else
			hasCollision = 1;

		if(currValidBits == 0){ 		// 当前UID全部是完整的字节，按字节拼接
			for(i=0;i<tmpSize;i++){
				uid[currValidBytes++] = g_rx_buff[i];
			}
			currValidBits = recBits;
		}
		else{							// 当前UID最后一个字节不完整，第一个字节需要按位拼接
			 if(recBytes == 0){ 			 // 仅收到一个字节新数据（可能也不完整）
				uid[currValidBytes-1] += g_rx_buff[0]<<currValidBits;	 // 按位拼接第一个字节
				currValidBits += recBits;
				if(currValidBits==8){
					currValidBytes++;
					currValidBits = 0;
				}
			}
			else{							// 收到多个字节数据
				uid[currValidBytes-1] |= g_rx_buff[0];//<<currValidBits;   // 按位拼接第一个字节
				for(i=1;i<tmpSize;i++){ 		  // 复制剩余的字节
					uid[currValidBytes++] = g_rx_buff[i];
				}
				currValidBits = recBits;
			}
		}
		if(hasCollision){

			if(currValidBits==0){
				uid[currValidBytes] = rand_bit;
				currValidBits = 1;
				NVB = 0x20+((currValidBytes)<<4) |	currValidBits;
				currValidBytes++;
			}
			else if(currValidBits == 7){
				uid[currValidBytes-1] += rand_bit<<7;
				currValidBits = 0;
				NVB = 0x20+((currValidBytes)<<4);
			}
			else{		// currValidBits= 1 ... 6
				uid[currValidBytes-1] += rand_bit<<currValidBits;
				currValidBits++;
				NVB = 0x20+((currValidBytes-1)<<4) |  currValidBits;
			}

			tmpBuf[0] = SEL;
			tmpBuf[1] = NVB;
			for(u8 i=0;i<currValidBytes;i++){
				tmpBuf[2+i] = uid[i];
			}
			sky1311WriteReg(pSky,ADDR_RX_START_BIT_NUM,currValidBits);
			if(sta_Ok != bitCollisionTrans(pSky,tmpBuf, currValidBytes+2 , currValidBits)){
				sky1311WriteReg(pSky,ADDR_RX_NUM_H, 0x00);// disable anti-collision
				sky1311WriteReg(pSky,ADDR_RX_START_BIT_NUM,0);
				return sta_Error;
			}
		}

	}while(hasCollision);
	sky1311WriteReg(pSky,ADDR_RX_NUM_H, 0x00); // disable anti-collision
	sky1311WriteReg(pSky,ADDR_RX_START_BIT_NUM,0);
	return sta_Ok;
}


/**
 ******************************************************************************
 ** \简  述  type A选卡
 **
 ** \参  数  BYTE SEL, 选择序列，93H，95H， 97H
 **          BYTE *uid, PICC 4 bytes UID
 **          BYTE *SAK
 ** \返回值  sta_result_t， 选卡状态值
 ******************************************************************************/
sta_result_t piccSelectA(PSky1311t pSky,BYTE SEL,BYTE *uid, BYTE *SAK)
{
    sta_result_t sta;
		WORD tmpSize;
		BYTE tmpBuf[10];

    sky1311WriteReg(pSky,ADDR_TX_CTRL, TX_CRC_EN|TX_PARITY_ODD|TX_POLE_HIGH); // TX odd parity, with CRC
    sky1311WriteReg(pSky,ADDR_RX_CTRL, RX_CRC_EN|RX_PARITY_EN|RX_PARITY_ODD);
    sky1311WriteReg(pSky,ADDR_TX_BIT_NUM, 0x08);                              // complete bytes

    tmpBuf[0] = SEL;
    tmpBuf[1] = 0x70;
    tmpBuf[2] = uid[0];
    tmpBuf[3] = uid[1];
    tmpBuf[4] = uid[2];
    tmpBuf[5] = uid[3];
    tmpBuf[6] = uid[4];
    //sta = data_tx_rx(7, TYPE_A, RATE_OFF);
	sky1311_fifo_tx(pSky,TYPE_A, tmpBuf, 7);
	sta = sky1311_fifo_rx(pSky,TYPE_A, RATE_OFF, SAK, &tmpSize);
    if(sta == sta_Ok && tmpSize == 3)
        return sta_Ok;
    else
        return sta_NoResponse;
}
/**
 ******************************************************************************
 ** \简  述  type A休眠,终止对卡的操作
 **
 ** \参  数  none
 ** \返回值  none
 ******************************************************************************/
void piccHaltA(PSky1311t pSky)
{
    sky1311WriteReg(pSky,ADDR_FIFO, 0x50);
    sky1311WriteReg(pSky,ADDR_FIFO, 0x00);
    sky1311WriteReg(pSky,ADDR_TX_BYTE_NUM, 0x02);
    sky1311WriteCmd(pSky,CMD_TX);                     // CMD TX
}
/*****************************************************************************/
/*                      以下是ISO14443-4内容  （备用）                        */
/*****************************************************************************/

#define TCL_I_BLOCK               0x02    //I-Block                             000x xx1x
#define TCL_I_BLOCK_RFU           0x02    //I-BLOCK_RFU                         00xx xx1x
#define TCL_R_BLOCK               0xA2    //R-Block                             1010 x01x
#define TCL_R_BLOCK_RFU           0xA0    //R-Block_RFU                         1010 x0xx
#define TCL_R_BLOCK_ACK           0xA2    //R-Block-ACK                         1010 x01x
#define TCL_R_BLOCK_ACK_RFU       0xA0    //R-Block-ACK_RFU                     1010 x0xx
#define TCL_R_BLOCK_NAK           0xB2    //R-Block-NAK                         1011 x01x
#define TCL_R_BLOCK_NAK_RFU       0xB0    //R-Block-NAK_RFU                     1011 x0xx
#define TCL_S_BLOCK_DES           0xC2    //S-Block                             1100 x010
#define TCL_S_BLOCK_DES_RFU       0xC0    //S-Block_RFU                         1100 x0xx
#define TCL_S_BLOCK_WTX           0xF2    //S-Block                             1111 x010
#define TCL_S_BLOCK_WTX_RFU       0xF0    //S-Block_RFU                         1111 x0xx
#define TCL_PPSS_BLOCK            0xD0    //PPS S-Block                         1101 xxxx
#define POWER_LEVEL_INDICATION    0x00    //CID byte                            xx00 0000
//#define TCL_S_BLOCK               0xC2    //S-Block                             11xx x010

#define TCL_M_I_BLOCK             0xE2    //I-BLOCK MASK                        1110 0010
#define TCL_M_I_BLOCK_RFU         0xC2    //I-BLOCK_RFU MASK                    1100 0010
#define TCL_M_R_BLOCK             0xE6    //R-BLOCK MASK                        1110 0110
#define TCL_M_R_BLOCK_RFU         0xE4    //R-BLOCK_RFU MASK                    1110 0100
//#define TCL_M_S_BLOCK             0xF6    //S-BLOCK MASK                        1111 0111
#define TCL_M_S_BLOCK             0xF7    //S-BLOCK MASK                        1111 0111
#define TCL_M_S_BLOCK_RFU         0xF4    //S-BLOCK_RFU MASK                    1111 0100
#define TCL_M_CHAINING            0x10    //CHAIN MASK                          0001 0000
#define TCL_M_CID                 0x08    //CID following                       0000 1000
#define TCL_M_NAD                 0x04    //NAD following                       0000 0100
#define TCL_M_BLK_NUM             0x01    //BLOCK NUM MASK                      0000 0001
#define TCL_M_NAK                 0x10    //R-NAK                               0001 0000
#define TCL_M_POWER_LEVEL         0x00    //Power Level Indication              0000 0000
#define TCL_M_WTXM                0x3F    //Waiting Time eXtension Multipier    0011 1111

BYTE  g_FWI;
BYTE  g_bCID;
WORD  g_bFSD;
WORD  g_bFSC;
BYTE  g_bBlockNum;
BYTE  g_bCIDFollowed;

BYTE  g_bLastTransmitPCB;
BYTE  * g_pbaLastTransmitBuf;
WORD g_wLastTransmitLength;
/**
 ******************************************************************************
 ** \简  述  取消选卡
 **
 ** \参  数  卡类型
 ** \返回值  sta_result_t，状态
 ******************************************************************************/
sta_result_t piccDeselectA(PSky1311t pSky)
{
    sta_result_t sta;
	WORD tmpSize;
	BYTE tmpBuf[2];
	BYTE g_rx_buff[4];

    tmpBuf[0] = 0xCA;
    tmpBuf[1] = 0x00;

	sky1311_fifo_tx(pSky,TYPE_A, tmpBuf, 2);

	sta = sky1311_fifo_rx(pSky,TYPE_A, RATE_OFF, g_rx_buff, &tmpSize);

    if(sta == sta_Ok && tmpSize == 4)
        return sta_Ok;
    else
        return sta_NoResponse;
}
/**
 ******************************************************************************
 ** \简  述  读取type A卡的ATS值
 **
 ** \参  数  none
 ** \返回值  sta_result_t，ATS值
 ******************************************************************************/

void getFSCFSD(BYTE bFSCIFSDI, BYTE bType)
{
    if(bFSCIFSDI > 8)
    {
        bFSCIFSDI = 8;
    }
    if(bType == 0)
    {
        switch(bFSCIFSDI)
        {
        case 0:
            g_bFSC = 16;
            break;

        case 1:
            g_bFSC = 24;
            break;

        case 2:
            g_bFSC = 32;
            break;

        case 3:
            g_bFSC = 40;
            break;

        case 4:
            g_bFSC = 48;
            break;

        case 5:
            g_bFSC = 64;
            break;

        case 6:
            g_bFSC = 96;
            break;

        case 7:
            g_bFSC = 128;
            break;

        case 8:
            g_bFSC = 256;
            break;
        }
    }
    else
    {
        switch(bFSCIFSDI)
        {
        case 0:
            g_bFSD = 16;
            break;

        case 1:
            g_bFSD = 24;
            break;

        case 2:
            g_bFSD = 32;
            break;

        case 3:
            g_bFSD = 40;
            break;

        case 4:
            g_bFSD = 48;
            break;

        case 5:
            g_bFSD = 64;
            break;

        case 6:
            g_bFSD = 96;
            break;

        case 7:
            g_bFSD = 128;
            break;

        case 8:
            g_bFSD = 256;
            break;
        }
    }
}

sta_result_t piccATS(PSky1311t pSky,BYTE *ATS)
{
	BYTE count;
    sta_result_t sta;
    WORD tmpSize;
    BYTE tmpBuf[2];
    count = 1;
    g_FWI = 4;
    tmpBuf[0] = 0xE0;
    tmpBuf[1] = 0x80;
    //sta = data_tx_rx(2, TYPE_A, RATE_OFF);

    sky1311_fifo_tx(pSky,TYPE_A, tmpBuf, 2);

    sta = sky1311_fifo_rx(pSky,TYPE_A, RATE_OFF, ATS, &tmpSize);

    if(sta == sta_Ok && (tmpSize == ATS[0] + 2))
    {
        g_bCID = tmpBuf[0] & 0x0F;

        if(ATS[1] & 0x10)
        {
            count++;
            g_FWI = ((ATS[2] >> 4) & 0x0F);
            if(g_FWI > 14)
            {
                g_FWI = 14;
            }
        }

        if(ATS[1] & 0x20)
        {
            count++;
        }

        if(ATS[1] & 0x40)
        {
            count++;

            if(ATS[count] & 0x02)
            {
                g_bCIDFollowed = 1;
            }
            else
            {
                g_bCIDFollowed = 0;
            }
        }
        else
        {
            g_bCIDFollowed = 0;
        }

        g_bBlockNum = 0;
        getFSCFSD((ATS[1] >> 4) & 0x0F, 0);
        getFSCFSD((tmpBuf[1] >> 4) & 0x0F, 1);

        //frame_delay = 32 * (2 << g_FWI) + 10;
        //timer0_value = (BYTE)(frame_delay & 0x0000ff);
        //timer1_value = (BYTE)((frame_delay >> 8) & 0x0000ff);
        //timer2_value = (BYTE)((frame_delay >> 16) & 0x00003f);
        //timer2_value = timer2_value + 0x40;
    }
    else
        return sta_NoResponse;
    return sta;
}

/**
 ******************************************************************************
 ** \简  述  读取PPS值
 **
 ** \参  数  设置的倍速
 ** \返回值  sta_result_t，PPS值
 ******************************************************************************/
sta_result_t piccPPS(PSky1311t pSky,WORD high_rate, BYTE *pps)
{
    sta_result_t sta;
    BYTE tmpBuf[3]={0xD0,0x11,0x00};
    WORD tmpSize;

    if(high_rate == 212)
        tmpBuf[2] = 0x05;                   // RATE = 212k
    else if(high_rate == 424)
        tmpBuf[2] = 0x0A;                   // RATE = 424k

    //sta = data_tx_rx(3, TYPE_A, RATE_OFF);
    sky1311_fifo_tx(pSky,TYPE_A, tmpBuf, 3);
    sta = sky1311_fifo_rx(pSky,TYPE_A, RATE_OFF, pps, &tmpSize);

    return sta;
}




