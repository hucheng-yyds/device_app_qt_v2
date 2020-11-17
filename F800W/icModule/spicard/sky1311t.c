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
 ** 文 件 名： sky1311s.c
 **
 ** 文件简述： 操作sky1311s的所有函数，常量和变量
 **
 ** 版本历史:
 ** 2015-10-05 V1.00  EH   第一个正式版本
 ** 2017-3-25  V.10  Release    更新代码和函数结构，重新发布
 **
 ******************************************************************************/

#include "sky1311t.h"
#include "userdef.h"
/**
 ******************************************************************************
 ** \简述： 全局变量定义
 **
 ******************************************************************************/
//sta_field_t pcdFieldState=NoObjectIn;      // 读卡器场状态（是否有物体在场内）
#if CHECKCARD==1
    BYTE maxRCADVal=MAXADVAL;         // 检测到的最大AD值
    BOOL freqScanNeed=TRUE;  // 系统需要RC频率扫描
#endif
void delay(volatile u8 i)
{
  while(i--)
  {
      //__no_operation();
      //__no_operation();
      //__no_operation();
      //__no_operation();  
  }
}

/////////////////////////五个驱动函数/////////////////////////////////////////////////////////
/******************************************************************************
 ** \简  述  通过SPI接口向1311写一个字节的命令
 **
 ** \参  数  命令字
 ** \返回值  none
 ******************************************************************************/
void sky1311WriteCmd(PSky1311t pSky,BYTE cmd)
{
    pSky->pConst->CsPinLow();
    cmd = (cmd & 0x1F) | 0x80;	        // bit7,6,5 = 100b, mean command
    //pSky->pConst->SpiSendByte(cmd);
    pSky->pConst->SpiSendCmd(cmd);
    pSky->pConst->CsPinHigh();

}
/**
 ******************************************************************************
 ** \简  述  通过SPI接口向1311的寄存器写一个字节数据
 **
 ** \参  数  BYTE regAdd: 寄存器地址， BYTE data: 要写入的数据
 ** \返回值  none
 ******************************************************************************/
void sky1311WriteReg(PSky1311t pSky,BYTE regAddress, BYTE udata)
{
    pSky->pConst->CsPinLow();
    regAddress      =   (regAddress & 0x3F);        // bit7,6=00, config as addr/write mode
    //pSky->pConst->SpiSendByte(regAddress);
    //pSky->pConst->SpiSendByte(udata);
    pSky->pConst->SpiSendByte(regAddress,udata);
    pSky->pConst->CsPinHigh();
}
/**
 ******************************************************************************
 ** \简  述  通过SPI接口读取1311的寄存器
 **
 ** \参  数  BYTE regAdd: 寄存器地址
 ** \返回值  BYTE 寄存器内容
 ******************************************************************************/
BYTE sky1311ReadReg(PSky1311t pSky,BYTE regAdd)
{
	BYTE value=0;
    pSky->pConst->CsPinLow();
    regAdd      =   (regAdd & 0x3F) | 0x40;            // bit7,6=01, config as addr/read mode
   // pSky->pConst->SpiSendByte(regAdd);
   // BYTE value = pSky->pConst->SpiSendByte(0xFF);
   value=pSky->pConst->SpiGetByte(regAdd,value);
  // pSky->pConst->SpiSendCmd(regAdd);
  // pSky->pConst->SpiSendByte(0xFF,value);

    pSky->pConst->CsPinHigh();
    return value;
}
/**
 ******************************************************************************
 ** \简  述  通过SPI接口向1311的FIFO写指定数目的数据
 **
 ** \参  数  BYTE* 数据内容头地址， BYTE count: 要写入的数据数量
 ** \返回值  none
 ******************************************************************************/
void sky1311WriteFifo(PSky1311t pSky,BYTE *udata, BYTE ucount)
{
    BYTE add;
    pSky->pConst->CsPinLow();
    add      =   (ADDR_FIFO & 0x3F);               // bit7,6=00, config as addr/write mode
   /* pSky->pConst->SpiSendByte(add);
    while(ucount--)
    {
      pSky->pConst->SpiSendByte(*udata++);
    }*/
    pSky->pConst->SpiSendBytes(add,udata,ucount);
    pSky->pConst->CsPinHigh();
}
/**
 ******************************************************************************
 ** \简  述  通过SPI接口向1311的FIFO读取指定数目的内容
 **
 ** \参  数  BYTE* data 保存读取内容的缓冲区首地址， BYTE count 读取的字节数
 ** \返回值  none
 ******************************************************************************/
void sky1311ReadFifo(PSky1311t pSky,BYTE *udata, BYTE ucount)
{
    BYTE add;
    pSky->pConst->CsPinLow();
    add   =   (ADDR_FIFO & 0x3F) | 0x40;            // bit7,6=01, config as addr/read mode
   /* pSky->pConst->SpiSendByte(add);
    while(ucount--)
    {
      *udata++ = pSky->pConst->SpiSendByte(0xFF);
    }*/
    pSky->pConst->SpiGetBytes(add,udata,ucount);
    pSky->pConst->CsPinHigh();
}
/////////////////////////////////////////////////////////////////////////////////////////
/**
 ******************************************************************************
 ** \简  述  设置寄存器掩码位
 **
 ** \参  数  BYTE regAddr 寄存器地址， BYTE mask 要设置的"掩码"字节
 ** \返回值  none
 ******************************************************************************/
void SetBitMask(PSky1311t pSky,BYTE regAddress, BYTE mask)
{
    BYTE tmp;
    tmp = sky1311ReadReg(pSky,regAddress);
    sky1311WriteReg(pSky,regAddress, tmp|mask);
}
/**
 ******************************************************************************
 ** \简  述  清除寄存器掩码位
 **
 ** \参  数  BYTE regAddr 寄存器地址， BYTE mask 要清除的"掩码"字节
 ** \返回值  none
 ******************************************************************************/
void ClearBitMask(PSky1311t pSky,BYTE regAddr, BYTE mask)
{
    BYTE tmp;
    tmp = sky1311ReadReg(pSky,regAddr);
    sky1311WriteReg(pSky,regAddr, tmp & ~mask);
}
/**
 ******************************************************************************
 ** \简  述  模拟参数初始化，配置模拟参数寄存器
 **
 ** \参  数  none
 ** \返回值  none
 ******************************************************************************/
void analogInit(PSky1311t pSky)
{
//	sky1311WriteReg(pSky,ADDR_ANA_CFG0, PA_3P3V);	 // 7-4: RC OSC Freq
//	sky1311WriteReg(pSky,ADDR_ANA_CFG0, PA_3P0V);	 // 7-4: RC OSC Freq
	sky1311WriteReg(pSky,ADDR_ANA_CFG0, PA_2P5V);	 // 7-4: RC OSC Freq
//	sky1311WriteReg(pSky,ADDR_ANA_CFG0, PA_2P0V);	 // 7-4: RC OSC Freq
											 // 3-2: PA Driver
											 //   1: ixtal
											 //   0: External LDO
	sky1311WriteReg(pSky,ADDR_ANA_CFG2, ANA2_A);
        //sky1311WriteReg(pSky,ADDR_ANA_CFG1, 0xEC);	  // 13.56M  7: txen
	sky1311WriteReg(pSky,ADDR_ANA_CFG1, 0x00);
    pSky->pConst->DelayMs(2);
    //sky1311WriteReg(pSky,ADDR_ANA_CFG1, 0xFC);	//27.12M	 7: txen
    sky1311WriteReg(pSky,ADDR_ANA_CFG1, 0xFC);	//27.12M	 7: txen
											 //   6: rxen
											 // 5-4: 00(osc off, xtal off) 01(osc on, xtal off) 10(osc off, xtal 13.56) 11(osc off, xtal 27.12)
}

/*static void analogInit(PSky1311t pSky)
{
    sky1311WriteReg(pSky,ADDR_ANA_CFG0, PA_2P5V);    // 7-4: RC OSC Freq
                                             // 3-2: PA Driver
                                             //   1: ixtal
                                             //   0: External LDO
    sky1311WriteReg(pSky,ADDR_ANA_CFG1, 0xFC);    //   7: txen
                                             //   6: rxen
                                             // 5-4: 00(osc off, xtal off) 01(osc on, xtal off) 10(osc off, xtal 13.56) 11(osc off, xtal 27.12)
    sky1311WriteReg(pSky,ADDR_ANA_CFG2, ANA2_A);

}
*/
/**
 ******************************************************************************
 ** \简  述  输出调试参数初始化
 **
 ** \参  数  选择通过MFOUT脚输出的信号，
 **          bit2,1,0
 **             0 0 0 rx phase signal after demod, before sample
 **             0 0 1 tx bit stream
 **             0 1 0 rx bit stream
 **             0 1 1 rx data from analog
 **             1 0 0 DO from digital
 ** \返回值  none
 ******************************************************************************/
/*static void directModeInit(BYTE mfout_sel)
{
    sky1311WriteReg(ADDR_MFOUT_SEL,  mfout_sel);
}*/

/**
 ******************************************************************************
 ** \简  述  打开PCD天线
 **
 ** \参  数  PCD类型， TYPE_A, TYPE_B
 ** \返回值  none
 ******************************************************************************/
void pcdAntennaOn(PSky1311t pSky,BYTE pcdType)
{
    if(pcdType == TYPE_A){
        sky1311WriteReg(pSky,ADDR_ANA_CFG2, ANA2_A);
    }else if(pcdType == TYPE_B){
        sky1311WriteReg(pSky,ADDR_ANA_CFG2, ANA2_B);
    }
    //SetBitMask(pSky,ADDR_TX_CTRL, TX_POLE);
    SetBitMask(pSky,ADDR_ANA_CFG1, TX_EN);
}

/**
 ******************************************************************************
 ** \简  述  关闭PCD天线
 **
 ** \参  数  none
 ** \返回值  none
 ******************************************************************************/
void pcdAntennaOff(PSky1311t pSky)
{
    //sky1311WriteReg(pSky,ADDR_ANA_CFG2, ANA2_A);
    //ClearBitMask(pSky,ADDR_TX_CTRL, TX_POLE);
    ClearBitMask(pSky,ADDR_ANA_CFG1, TX_EN);  // Disable TX
}
/**
 ******************************************************************************
 ** \简  述  清除所有中断标记
 **
 ** \参  数  none
 ** \返回值  none
 ******************************************************************************/
void irqClearAll(PSky1311t pSky)
{
    sky1311WriteReg(pSky,ADDR_IRQ_STA, 0xFF);
}
/**
 ******************************************************************************
 ** \简  述  清除指定的中断标记
 **
 ** \参  数  BYTE irq
 ** \返回值  none
 ******************************************************************************/
void irqClear(PSky1311t pSky,BYTE irq)
{
    sky1311WriteReg(pSky,ADDR_IRQ_STA,irq);
}
/**
 ******************************************************************************
 ** \简  述  设置sky1311与卡通信的倍速, ISO14443-4部分规定，多倍速的PICC
 **
 ** \参  数  设置的倍速，106，212或424
 ** \返回值  none
 ******************************************************************************/
void bitRateSet(PSky1311t pSky,WORD high_rate)
{
    sky1311WriteReg(pSky,ADDR_FSM_STATE, RATE_SEL);                           // enable high-speed mode
    if(high_rate == 212){
        sky1311WriteReg(pSky,ADDR_TX_CTRL, TX_212|TX_POLE_HIGH|TX_CRC_EN|TX_PARITY_ODD);
        sky1311WriteReg(pSky,ADDR_RATE_CTRL, RX_RATE_WID_2|RX_RATE_212|RX_RATE_PAR_ODD|RX_RATE_CRC_EN|RX_RATE_PAR_EN);
        sky1311WriteReg(pSky,ADDR_TX_PUL_WID,0x14);                           // tx pulse width = 1.5u
        sky1311WriteReg(pSky,ADDR_RATE_FRAME_END,0x40);                       // stop counter = 0x40
        sky1311WriteReg(pSky,ADDR_RATE_THRES,0x20);                           // threshold = 0x20
    }else if(high_rate == 424){
        sky1311WriteReg(pSky,ADDR_TX_CTRL, TX_424|TX_POLE_HIGH|TX_CRC_EN|TX_PARITY_ODD);
        sky1311WriteReg(pSky,ADDR_RATE_CTRL, RX_RATE_WID_2|RX_RATE_424|RX_RATE_PAR_ODD|RX_RATE_CRC_EN|RX_RATE_PAR_EN);
        sky1311WriteReg(pSky,ADDR_TX_PUL_WID,0x0A);                           // tx pulse width = 0.8u
        sky1311WriteReg(pSky,ADDR_RATE_FRAME_END,0x20);                       // stop counter = 0x20
        sky1311WriteReg(pSky,ADDR_RATE_THRES,0x10);                           // threshold = 0x10
    }
}
/**
 ******************************************************************************
 ** \简  述  选择type A 卡作为操作对象
 **
 ** \参  数  none
 ** \返回值  none
 ******************************************************************************/
void typeAOperate(PSky1311t pSky)
{
    sky1311WriteReg(pSky,ADDR_ANA_CFG2, ANA2_A);             // analogA select
    sky1311WriteReg(pSky,ADDR_FSM_STATE, TYPE_A_SEL);        // typeA select
    sky1311WriteReg(pSky,ADDR_CRC_CTRL, CRC_A);              // crcA enable
    sky1311WriteReg(pSky,ADDR_TX_PUL_WID,0x26);              // set to default value
    sky1311WriteReg(pSky,ADDR_M1_CTRL, 0x00);                // disable M1 operation
    //sky1311WriteReg(pSky,ADDR_ANA_CFG3, 0x80);
    sky1311WriteReg(pSky,ADDR_ANA_CFG3, 0x09);
    sky1311WriteReg(pSky,ADDR_ANA_CFG3, 0x81);
}
/**
 ******************************************************************************
 ** \简  述  选择type B 卡作为操作对象
 **
 ** \参  数  none
 ** \返回值  none
 ******************************************************************************/
void typeBOperate(PSky1311t pSky)
{
    sky1311WriteReg(pSky,ADDR_ANA_CFG2, ANA2_B);            // analogB select
    sky1311WriteReg(pSky,ADDR_FSM_STATE, TYPE_B_SEL);       // typeB select
    sky1311WriteReg(pSky,ADDR_CRC_CTRL, CRC_B);             // crcB enable
    sky1311WriteReg(pSky,ADDR_TX_PUL_WID, 0x26);             // crcB enable
    sky1311WriteReg(pSky,ADDR_M1_CTRL, 0x00);             // crcB enable
    sky1311WriteReg(pSky,ADDR_ANA_CFG3, 0x09);             // crcB enable
    sky1311WriteReg(pSky,ADDR_ANA_CFG3, 0x81);             // crcB enable
}
u8 sky1311ReadBuff[0x40];
void sky1311ReadAll(PSky1311t pSky)
{
	u8 i=0;
	for(i=0;i<0x40;i++)
	{
		sky1311ReadBuff[i] = sky1311ReadReg(pSky,i);		 // time_out timer stop condition = beginning of RX SOF
	}
	
}
/**
 ******************************************************************************
 ** \简  述  初始化SKY1311S
 **
 ** \参  数  none
 ** \返回值  none
 ******************************************************************************/
void sky1311Init(PSky1311t pSky)
{
    sky1311WriteCmd(pSky,CMD_SW_RST);                // reset all register

    analogInit(pSky);
    //sky1311WriteReg(pSky,ADDR_MFOUT_SEL, 0x44);       // time_out timer stop condition = beginning of RX SOF
    pSky->pConst->DelayMs(3);  //Crystal Stable time
    //directModeInit(MFOUT_ANALOG_RX);
    sky1311WriteReg(pSky,ADDR_TIME_OUT2, 0x8F);       // time_out timer stop condition = beginning of RX SOF
    sky1311WriteReg(pSky,ADDR_TIME_OUT1, 0xFF);
    sky1311WriteReg(pSky,ADDR_TIME_OUT0, 0xFF);
    //sky1311WriteReg(ADDR_RX_PUL_DETA, 0x44);
    //sky1311WriteReg(pSky,ADDR_RX_PUL_DETA, 0x64);     // 高4位识别曼彻斯特码脉宽，越大容错能力越强
    sky1311WriteReg(pSky,ADDR_RX_PUL_DETA, 0x34);     // 高4位识别曼彻斯特码脉宽，越大容错能力越强
    //sky1311WriteReg(ADDR_RX_PRE_PROC, 0x26);
    sky1311WriteReg(pSky,ADDR_RX_PRE_PROC, 0x00);
    sky1311WriteReg(pSky,ADDR_RX_START_BIT_NUM,0);
    sky1311WriteReg(pSky,ADDR_MOD_SRC,0x02);
    sky1311WriteReg(pSky,ADDR_IRQ_EN, IRQ_M1_EN|IRQ_TOUT_EN|IRQ_TX_EN|IRQ_RX_EN|IRQ_HIGH_EN|IRQ_LOW_EN);    // enable all IRQ
    sky1311WriteReg(pSky,ADDR_FIFO_CTRL,8);            // set water-level of FIFO
    /*sky1311WriteReg(pSky,ADDR_RX_NUM_L, 0x05); 
    sky1311WriteReg(pSky,ADDR_TIMER_CNT0, 0x0A); 
    sky1311WriteReg(pSky,ADDR_RATE_RX_BYTE, 0x05); 
    sky1311WriteReg(pSky,ADDR_M1_CTRL, 0x00); 
    sky1311WriteReg(pSky,ADDR_RX_B_BYTE_NUM, 0x05); 
	*/
}
/**
 ******************************************************************************
 ** \简  述  SKY1311S复位，并处于disable状态
 **
 ** \参  数  none
 ** \返回值  none
 ******************************************************************************/
void sky1311Reset(PSky1311t pSky)
{
    sky1311WriteReg(pSky,ADDR_ANA_CFG1, 0x04);   // close TX, RX, OSC off
    sky1311WriteCmd(pSky,CMD_SW_RST);
    pSky->pConst->DisablePdPin();
    //SKY1311_DISABLE();
}

/**
 ******************************************************************************
 ** \简  述  复位PICC，关闭场5ms,再打开
 **
 ** \参  数  none
 ** \返回值  none
 ******************************************************************************/
void resetPicc(PSky1311t pSky)
{
    ClearBitMask(pSky,ADDR_ANA_CFG1,TX_EN);
    pSky->pConst->DelayMs(5);
    SetBitMask(pSky,ADDR_ANA_CFG1,TX_EN);
    pSky->pConst->DelayMs(10);

}
/**
 ******************************************************************************
 ** \简  述  数据发射函数，将数据写到FIFO中并通过射频接口发送给PICC
 **
 ** \参  数   BYTE txType,        // 发射类型（TYPE_A | TYPE_B）
 **           BYTE *txBuff,      // 数据内容
 **           WORD txSize       // 数据大小
 ** \返回值  none
 ******************************************************************************/
void sky1311_fifo_tx(PSky1311t pSky,BYTE txType, BYTE *txBuff, WORD txSize)
{
    BYTE  irq_sta;   // 中断请求状态代码
    WORD tx_res = txSize;
    volatile WORD delayCount;
    sky1311WriteCmd(pSky,CMD_IDLE);              // reset state machine to Idle mode
    sky1311WriteCmd(pSky,CMD_CLR_FF);            // clear FIFO
    irqClearAll(pSky);                          // clear all IRQ state

    //sky1311WriteReg(ADDR_FIFO_CTRL,8);      // set water-level of FIFO
    /* write numbers */
    if(txType == TYPE_A){
        sky1311WriteReg(pSky,ADDR_TX_BYTE_NUM, txSize & 0x00ff);
        if(txSize > 255)
            sky1311WriteReg(pSky,ADDR_TX_BIT_NUM, ((txSize & 0x0f00)>>4|8));// write the length to tx_byte register
    }else if(txType == TYPE_B){
        sky1311WriteReg(pSky,ADDR_TX_B_BYTE_NUM, txSize & 0x00ff);
        if(txSize > 255)
            sky1311WriteReg(pSky,ADDR_TX_B_EGT_NUM, (txSize & 0x0300)>>4);
    }
    /* when TX length<=FIFO's depth, write all data to FIFO */
    if( txSize <= 64){
        sky1311WriteFifo(pSky,txBuff, txSize);
        sky1311WriteCmd(pSky,CMD_TX_RX);             // transceive & into receive mode
    }
    /* when TX length > FIFO's depth */
    else{               // txLen > 64
        sky1311WriteFifo(pSky,txBuff, 64);       // send 64 bytes
        sky1311WriteCmd(pSky,CMD_TX_RX);
        tx_res = txSize - 64;
        while(tx_res>0){                    // send remain bytes
            if(pSky->pConst->IrqReadPin()){// HAL_GPIO_ReadPin(IRQ_1311_GPIO_Port,IRQ_1311_Pin)){
                irq_sta = sky1311ReadReg(pSky,ADDR_IRQ_STA);
                if(irq_sta & IRQ_LOW){  // FIFO low
                    if(tx_res>=56){
                        sky1311WriteFifo(pSky,&txBuff[txSize - tx_res], 56);
                        tx_res -=56;
                    }
                    else{
                        sky1311WriteFifo(pSky,&txBuff[txSize - tx_res], tx_res);
                        tx_res = 0;
                    }
                    //irqClearAll(pSky);
                    irqClear(pSky,irq_sta);
                }
                else        // error
                {
                    //irqClearAll(pSky);
                    irqClear(pSky,irq_sta);
                }	
            }
        }
    }
    /* wait TX finished */

    while(1){
        delayCount = 0x4FFF;
        //while(!HAL_GPIO_ReadPin(IRQ_1311_GPIO_Port,IRQ_1311_Pin) && delayCount--);   // waiting for TX STOP IRQ
        while(!pSky->pConst->IrqReadPin() && --delayCount);   // waiting for TX STOP IRQ
        //if(!HAL_GPIO_ReadPin(IRQ_1311_GPIO_Port,IRQ_1311_Pin))
        irq_sta=sky1311ReadReg(pSky,ADDR_IRQ_STA);
		if((delayCount==0)||(irq_sta & IRQ_TOUT))
		{
			sky1311WriteCmd(pSky,CMD_IDLE);
            //irqClearAll();
            return;
		}
        if(irq_sta & IRQ_TX){
            irqClear(pSky,IRQ_TX);
            return;
        }
        else{
            irqClear(pSky,irq_sta);
        }
    }
}
/**
 ******************************************************************************
 ** \简  述  数据接收函数，将FIFO中数据读出来
 **
 ** \参  数   BYTE txType,        // 发射类型（TYPE_A | TYPE_B）
              BYTE rateType,      // 是否多倍速
 **           BYTE *rxBuff,      // 数据内容
 **           WORD rxSize       // 数据大小
 ** \返回值  Ok,正确接收到数据 ; 其它，产生错误
 ******************************************************************************/
sta_result_t sky1311_fifo_rx(PSky1311t pSky,BYTE rxType, BYTE rateType, BYTE *rxBuff, WORD *rxSize)
{
    WORD rx_buf_cnt=0;
    BYTE  byte_num_H;
    BYTE  byte_num_L;
    BYTE  temp_len;
    BYTE  bit_n=0;
    BYTE  err_sta;   // 错误状态代码
    BYTE  irq_sta;   // 中断请求状态代码


    volatile WORD delayCount;

    while(1){
        delayCount=0x4FFF;                        // delay
        while(!pSky->pConst->IrqReadPin() && --delayCount);
        irq_sta = sky1311ReadReg(pSky,ADDR_IRQ_STA);
        err_sta = sky1311ReadReg(pSky,ADDR_ERR_STA);
        if((delayCount==0)||(irq_sta & IRQ_TOUT)){             // tiemout
            sky1311WriteCmd(pSky,CMD_IDLE);
            irqClearAll(pSky);
            return sta_NoResponse;
        }
        else if(irq_sta & IRQ_HIGH){        // FIFO High
            sky1311ReadFifo(pSky,&rxBuff[rx_buf_cnt], 56);   // load next 56 bytes into FIFO
            rx_buf_cnt += 56;
            irqClear(pSky,IRQ_HIGH);//irqClearAll();
        }
        else if(irq_sta & IRQ_RX){          // Received
            if( ((sky1311ReadReg(pSky,ADDR_FIFO_LEN))<1) || (err_sta & 0xC0) ){
                sky1311WriteCmd(pSky,CMD_CLR_FF);           // noise occur, restart the rx
                sky1311WriteCmd(pSky,CMD_RX);
                irqClear(pSky,IRQ_RX);//irqClearAll(pSky);
                return sta_Error;
            }
            else{
                irqClearAll(pSky);
                if(err_sta & 0xc0)
                    return sta_Error;
				
                temp_len = sky1311ReadReg(pSky,ADDR_FIFO_LEN);               // get FIFO length
				//printf("temp_len =%x %x\n",temp_len,err_sta);

				sky1311ReadFifo(pSky,&rxBuff[rx_buf_cnt], temp_len);        // get data ,FIFO-->rx_buf
                rx_buf_cnt += temp_len;

                if(rxType == TYPE_A){
                    if(rateType){
                        byte_num_H = sky1311ReadReg(pSky,ADDR_RATE_RX_BIT);
                        byte_num_L = sky1311ReadReg(pSky,ADDR_RATE_RX_BYTE);
                    }else{
                        byte_num_H = sky1311ReadReg(pSky,ADDR_RX_NUM_H);
                        byte_num_L = sky1311ReadReg(pSky,ADDR_RX_NUM_L);
                        bit_n = ((sky1311ReadReg(pSky,ADDR_RX_NUM_H)) & 0xf0)>>4;
                        if(bit_n) sky1311ReadFifo(pSky,&rxBuff[rx_buf_cnt], 1);
                    }
                        *rxSize  = ((byte_num_H & 0x01) << 8) | byte_num_L;

                        if(bit_n)
                        {
                            *rxSize = ((*rxSize) + 1);
                        }
                }
                else if(rxType == TYPE_B){
                    byte_num_H = sky1311ReadReg(pSky,ADDR_RX_B_CTRL);
                    byte_num_L = sky1311ReadReg(pSky,ADDR_RX_B_BYTE_NUM);
                    *rxSize  = ( (byte_num_H & 0x80) << 1 ) | byte_num_L;
                }

                return sta_Ok;
            }
        }
        else if(irq_sta & IRQ_LOW){    //清除IRQ_LOW中断
          irqClear(pSky,IRQ_LOW);
        }
        else{
            irqClearAll(pSky);
            return sta_NoResponse;
        }
    }
}
sta_result_t ExchangeData(PSky1311t pSky,u8 cardType, u8 rateType,
                    u8 *pSendDataBuf, u16 ucSendLen,
                    u8 *pRcvDataBuf,  u16 *pRcvLen)
{
    sta_result_t sta;
    sky1311_fifo_tx(pSky,cardType, pSendDataBuf, ucSendLen);
    sta = sky1311_fifo_rx(pSky,cardType, rateType, pRcvDataBuf, pRcvLen);
    return sta;
}

/**
 ******************************************************************************
 ** \简  述  发送数据后立即转入接收数据
 **          要发送到数据保存在全局变量tx_buf中（发送缓冲区）
 **          接收的数据保存在全局变量rx_buf中，接收到的数量保存在变量rx_len中
 ** \参  数  BYTE txLen 发送数据的长度，最多255字节
 **          BYTE tx_rx_type 发送和接收的PICC类型 （TYPE_A | TYPE_B）
 **          BYTE rate_type 是否使用多倍速
 ** \返回值
 ******************************************************************************/
#if 0
sta_result_t data_tx_rx(BYTE txLen, BYTE tx_rx_type, BYTE rate_type)
{
    sta_result_t sta;
    tx_func(txLen, tx_rx_type);
    sta = rx_func(tx_rx_type, rate_type);
    return sta;
}
#endif

/**
 ******************************************************************************
 ** \简  述  Type A 防冲突数据发送函数，将数据写到FIFO中并通过射频接口发送给PICC
 **          发送的数据保存在全局变量tx_buf中，发送缓冲区
 **
 ** \参  数  BYTE* txBuf           数据缓冲区
 **          BYTE txLen          发射的数据长度（包括不完整的部分）,
 **          BYTE lastBitNum     最后一个字节有效位数
 ** \返回值  Ok--成功发射，Timeout--超时, Error -- 其它错误
 ******************************************************************************/
sta_result_t bitCollisionTrans(PSky1311t pSky,BYTE* txBuf, BYTE txLen, BYTE lastBitNum)
{
    BYTE  irq_sta;   // 中断请求状态代码
    volatile WORD delayCount;
    if(txLen>7)          // The maximum length of for transmission from PCD to PICC shall be 55 data bits
        return sta_Error;
    sky1311WriteCmd(pSky,CMD_IDLE);              // reset state machine to Idle mode
    sky1311WriteCmd(pSky,CMD_CLR_FF);            // clear FIFO
    irqClearAll(pSky);                          // clear all IRQ state
    /* write numbers */
    sky1311WriteReg(pSky,ADDR_TX_BYTE_NUM, txLen);
    sky1311WriteReg(pSky,ADDR_TX_BIT_NUM,lastBitNum);
    sky1311WriteFifo(pSky,txBuf, txLen);         // write data to FIFO
    sky1311WriteCmd(pSky,CMD_TX_RX);             // transceive & into receive mode
    /* wait TX finished */
    while(1){
        delayCount = 0x4FFF;                // timeout count
        while(! pSky->pConst->IrqReadPin() && delayCount--);   // waiting for TX STOP IRQ
        //if(! pSky->pConst->IrqReadPin())
        //    return sta_Timeout;
        irq_sta = sky1311ReadReg(pSky,ADDR_IRQ_STA);
        if((delayCount==0)||(irq_sta & IRQ_TOUT)){
            sky1311WriteCmd(pSky,CMD_IDLE);
            irqClearAll(pSky);
            return sta_Timeout;
        }
        else if(irq_sta & IRQ_TX){
            irqClear(pSky,IRQ_TX);
            break;
        }
        else{
            irqClearAll(pSky);
        }
    }
    /* 等待接收数据完成 */
    delayCount = 0x4FFF;                            // timeout count
    while(! pSky->pConst->IrqReadPin() && delayCount--);               // waiting for TX STOP IRQ
    //if(! pSky->pConst->IrqReadPin())
    //    return sta_Timeout;
    irq_sta = sky1311ReadReg(pSky,ADDR_IRQ_STA);
    if((delayCount==0)||(irq_sta & IRQ_TOUT)){
        sky1311WriteCmd(pSky,CMD_IDLE);
        irqClearAll(pSky);
        return sta_Timeout;
    }else if(!(irq_sta & IRQ_RX)){
        irqClearAll(pSky);
        return sta_Error;
    }
    irqClearAll(pSky);
    return sta_Ok;
}


/**
 ******************************************************************************
 ** \简  述  扫描最优的RC频率，为低功耗询卡用，
 **          注意：需要在读卡器场内没有任何导电物质遮挡时扫描
 **
 ** \参  数  none
 ** \返回值  低8位采集到的最大AD值，
         bit8--bit11 表示幅值最大时的频率值，对应Analog0寄存器的bit7--bit4
   		 bit12 表示是否扫描到最大值，1--扫描到，0--没有。
   		 bit13--15 没有使用，设置为0
 ******************************************************************************/
WORD sky1311RCFreqCali(PSky1311t pSky)
{
    BYTE currAdcVal=0;       // 当前AD值
    BYTE maxAdcVal=0;        // 最大AD值
    BYTE currRCFreqVal=0;     // 当前频率值
    BYTE MaxRCFreqVal = 0;    // 最大的频率值
    pSky->pConst->EnablePdPin();//sky1311Enable();
    pSky->pConst->DelayMs(5);
    sky1311WriteReg(pSky,ADDR_ANA_CFG1, 0x1C); //OSC Enable
    sky1311WriteReg(pSky,ADDR_ANA_CFG2, 0xA0);
    for(currRCFreqVal=0;currRCFreqVal<16;currRCFreqVal++)   // from 0000 to 1111 scan
    {
        sky1311WriteReg(pSky,ADDR_ANA_CFG0, (currRCFreqVal << 4) | PA_2P0V);
        sky1311WriteReg(pSky,ADDR_ANA_CFG4, 0x00);
        sky1311WriteReg(pSky,ADDR_ANA_CFG4, 0x40);        
        delay(30);
        sky1311WriteReg(pSky,ADDR_ANA_CFG4, 0x60);        
        delay(10);
        sky1311WriteReg(pSky,ADDR_ANA_CFG4, 0x70);        
        delay(10);
        sky1311WriteReg(pSky,ADDR_ANA_CFG4, 0x50);        
        delay(20);
        sky1311WriteReg(pSky,ADDR_ANA_CFG4, 0x00);        
        delay(10);
		currAdcVal = sky1311ReadReg(pSky,ADDR_ANA_CFG5); // read ADC value from analog5 register
		
		//DrvPrintf("Effi Adc=%d",currAdcVal);
		
        if(currAdcVal > maxAdcVal){			// 当前频率的AD值若大于最大值
            maxAdcVal = currAdcVal; 			// 用当前值取代最大值
            MaxRCFreqVal = currRCFreqVal;   	// 记下当前频率值
        }
        //pSky->pConst->DelayMs(1);//delay(50);
	}
    if(maxAdcVal>MINADVAL){
        maxRCADVal = maxAdcVal;

        //DrvPrintf("\r\n--RC frequency calibate, RC Param = %d",MaxRCFreqVal);
        //DrvPrintf("    AD Value=%d\r\n",maxRCADVal);

        return ((0x10 | MaxRCFreqVal) << 8 | maxAdcVal);		// 返回成功标记和频率值
    }
    else{
        maxRCADVal = MAXADVAL;
        //DrvPutStr("\r\n--RC frequency calibrate: Error!\r\n");
        return 0;
    }
}


/**
 ******************************************************************************
 ** \简  述  检查是否有卡进入或者离开场(低功耗询卡)
 **
 ** \参  数  BYTE RC频率设定值
 ** \返回值  没有动作，0  (设置低功耗询卡标记)
 **          有卡进入，1  (设置读卡标记)
 **          有卡离开，2  (设置重新校准查询卡频率标记)
 ******************************************************************************/
void checkCardInit(PSky1311t pSky,WORD rc_val)
{
    BYTE MaxRCFreqVal, MaxADCVal;

    MaxRCFreqVal = (BYTE)((rc_val >> 4) & 0xF0); // RC频率校准值

    MaxADCVal = (BYTE)((rc_val & 0xFF));         // 对应的最大AD值
    sky1311WriteReg(pSky,ADDR_ANA_CFG1, 0x00);

    /* RC校准时获得的AD值可能比实际自动检卡时的AD值偏大，
    如果这样可以将ＭaxADCVal值减去一定的值，减去的值根据实际情况确定 */
    //MaxADCVal -= 10;

    pSky->pConst->EnablePdPin();//sky1311Enable();
    pSky->pConst->DelayMs(3);
    sky1311WriteReg(pSky,ADDR_ANA_CFG0, MaxRCFreqVal | PA_2P0V);
    sky1311WriteReg(pSky,ADDR_ANA_CFG2, 0x10);
    sky1311WriteReg(pSky,ADDR_ANA_CFG4, 0x00);
    sky1311WriteReg(pSky,ADDR_ANA_CFG7, ADC_SAMPLE_5US | TX_SETTLE_0US | RSSI_DELTA);
    sky1311WriteReg(pSky,ADDR_ANA_CFG8, MaxADCVal-RSSI_ABS);
    sky1311WriteReg(pSky,ADDR_ANA_CFG6, WKU_EN | WKU_OR | WKU_300MS | RSSI_2AVG);
    irqClearAll(pSky);
    sky1311WriteReg(pSky,ADDR_ANA_CFG1, 0x1C);
    pSky->pConst->DisablePdPin();//SKY1311_DISABLE();
}

void checkCardDeinit(PSky1311t pSky)
{
    pSky->pConst->EnablePdPin();//SKY1311_ENABLE();
    pSky->pConst->DelayMs(5);
    sky1311WriteReg(pSky,ADDR_ANA_CFG6, 0x22);
    sky1311WriteReg(pSky,ADDR_ANA_CFG7, 0x3F);
    sky1311WriteReg(pSky,ADDR_ANA_CFG8, 0xFF);
    pSky->pConst->DisablePdPin();//      SKY1311_DISABLE();
}

/**
 ******************************************************************************
 ** \简  述  检查是否有卡离开场配置
 **
 ** \参  数  WORD RC频率扫描返回值
 ** \返回值  无
 ******************************************************************************/

/**
 ******************************************************************************
 ** \简  述  检查是否有卡离开场
 **
 ** \参  数  BYTE RC频率扫描得到的最大AD值
 ** \返回值  true,卡已经离开； false， 卡没有离开
 ******************************************************************************/
u8 maxRCFreqVal;
BOOL checkCardRemove(PSky1311t pSky,u16 rc_val)
{
    u8 maxAdcVal;
    BYTE currAdcVal;       // 当前AD值
    maxRCFreqVal = (u8)((rc_val >> 4) & 0xF0);
    maxAdcVal = (u8)(rc_val & 0xff);
    pSky->pConst->EnablePdPin();//sky1311Enable();
    pSky->pConst->DelayMs(5);
    sky1311WriteReg(pSky,ADDR_ANA_CFG1, 0x1C);
    sky1311WriteReg(pSky,ADDR_ANA_CFG2, 0xA0);
    sky1311WriteReg(pSky,ADDR_ANA_CFG0, maxRCFreqVal | PA_2P0V);
    sky1311WriteReg(pSky,ADDR_ANA_CFG4, 0x00);
    sky1311WriteReg(pSky,ADDR_ANA_CFG4, 0x40);        
    delay(30);
    sky1311WriteReg(pSky,ADDR_ANA_CFG4, 0x60);        
    delay(10);
    sky1311WriteReg(pSky,ADDR_ANA_CFG4, 0x70);        
    delay(10);
    sky1311WriteReg(pSky,ADDR_ANA_CFG4, 0x50);        
    delay(20);
    sky1311WriteReg(pSky,ADDR_ANA_CFG4, 0x00);        
    delay(10);
	currAdcVal = sky1311ReadReg(pSky,ADDR_ANA_CFG5); // read ADC value from analog5 register
    //DrvPrintf("RC Freq index=%d\r\n",maxRCFreqVal);
    //DrvPrintf("checkCardRemove AD=%d\r\n",currAdcVal);
    pSky->pConst->DisablePdPin();//sky1311Disable();
    if(currAdcVal > (maxAdcVal - RSSI_ABS))
    {
        return TRUE;
    }
    else
        return FALSE;

}

///////////////////////////////////////////////////////////////
void IsrSky1311Drv1msTimebase(PSky1311t pSky){
	if (pSky->pVar->wMoveCardTimeOut){
		pSky->pVar->wMoveCardTimeOut--;
	}
}

