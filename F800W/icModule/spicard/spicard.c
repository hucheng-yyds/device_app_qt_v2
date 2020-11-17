/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 
 ***create** zcl
 */
#include <stdio.h>  
#include <string.h>  
#include <sys/types.h>  
#include <errno.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <unistd.h>  
#include <termios.h>  
#include <stdlib.h> 
	   
#include <unistd.h>
	
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/types.h>
	
#include <sys/epoll.h>
#include <sys/ioctl.h>

#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <getopt.h>
#include<signal.h>


#include "sky1311Instan.h"

#include "spicard.h"


#define BCM53115M_SPI_STATUS_REG 0xFE/*RO*/
#define BCM53115M_SPI_STATUS_SPIF 0x80
#define BCM53115M_SPI_STATUS_RACK 0x20
#define BCM53115M_SPI_STATUS_MDIO 0x04
#define BCM53115M_SPI_PAGE_REG 0xFF/*R/W*/
#define BCM53115M_SPI_DATA_REG 0xF0/*R/W*/

#ifndef ERROR
#define ERROR (-1)
#endif
#ifndef OK
#define OK (0)
#endif

int pgpiofd=0;
int spifd_1311=0;


pthread_mutex_t lock;      //互斥LOCK
pthread_cond_t notempty;   //缓冲区非空条件判断




//static const char *device = "/dev/spidev1.0";
static const char *spiDevice0 = "/dev/spidev1.0";
static const char *spiDevice1 = "/dev/spidev1.1";
static const char *spiDevice2 = "/dev/spidev32766.2";
static u8 mode ;
static u8 bits ;
static u32 speed ;
bool esc = false;
char getcardid = 0;
extern TagInfoType TagInfo;

int SPI_WriteCmd_1311(int fd, u8 writedata)
{
	int ret;
	u8 writeCmd[1];
	struct spi_ioc_transfer xfer[1];

	writeCmd[0] = writedata;
	memset(xfer, 0, 1*sizeof(struct spi_ioc_transfer));

	xfer[0].tx_buf = (unsigned long)writeCmd;
	xfer[0].len = 1;
	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
	if (ret < 1)
    {
//        printf("can't xfer spi message1\n");
    }
	return OK;
}

int SPI_WriteByte_LSB_1311(int fd, u8 writeAddress, u8 writedata)
{
	int ret;
	u8 writeCmd[1];
	u8 spiStateReg[1];
	//u8 rev[1];
	struct spi_ioc_transfer xfer[3];

	//writeCmd[0] = 0x03;
	spiStateReg[0] = writeAddress;
	writeCmd[0] = writedata;

	memset(xfer, 0, 3*sizeof(struct spi_ioc_transfer));

//	xfer[0].tx_buf = (unsigned long)writeCmd;
//	xfer[0].len = 1;

	xfer[0].tx_buf = (unsigned long)spiStateReg;
	xfer[0].len = 1;

	xfer[1].tx_buf = (unsigned long)writeCmd;
	xfer[1].len = 1;


	ret = ioctl(fd, SPI_IOC_MESSAGE(2), &xfer);
//	if (ret < 1)
//		printf("can't xfer spi message1");
	return OK;
}

int SPI_WriteBytes_LSB_1311(int fd, u8 spireg, u8 *buf, u8 length)
{
	int ret,i;
//	u8 writeCmd[1];
	u8 spiStateReg[1];
//	u8 receiveData[1];
	struct spi_ioc_transfer xfer[3];
#if 0
	writeCmd[0] = 0x02;
	spiStateReg[0] = spireg;
	memset(xfer, 0, 3*sizeof(struct spi_ioc_transfer));

	xfer[0].tx_buf = (unsigned long)writeCmd;
	xfer[0].len = 1;

	xfer[1].tx_buf = (unsigned long)spiStateReg;
	xfer[1].len = 1;
	xfer[2].tx_buf = (unsigned long)buf;
	xfer[2].len = length;
	ret = ioctl(fd, SPI_IOC_MESSAGE(3), &xfer);
	if (ret < 1)
		printf("can't xfer spi message1");

#endif
#if 1
		spiStateReg[0] = spireg;		
		memset(xfer, 0, 2*sizeof(struct spi_ioc_transfer));
		
		xfer[0].tx_buf = (unsigned long)spiStateReg;
		xfer[0].len = 1;
		
		
		xfer[1].tx_buf = (unsigned long)buf;
		xfer[1].len =length;
		
		ret = ioctl(fd, SPI_IOC_MESSAGE(2), &xfer);
        if (ret < 1){
//			printf("can't xfer spi message1");
		}			
#endif	
	return OK;
}

u8 SPI_ReadByte_LSB_1311(int fd, u8 readAddress,u8 udata)
{
	int ret;
	u8 readCmd[1];
	u8 spiStateReg[1];
	u8 receiveData[1];
	struct spi_ioc_transfer xfer[3];
#if 0
	readCmd[0] = 0x03;
	spiStateReg[0] = readAddress;
	receiveData[0]=55;

	memset(xfer, 0, 2*sizeof(struct spi_ioc_transfer));

	xfer[0].tx_buf = (unsigned long)readCmd;
	xfer[0].len = 1;


	xfer[1].tx_buf = (unsigned long)spiStateReg;
	xfer[1].rx_buf = (unsigned long)receiveData;
	xfer[1].len = 1;

	ret = ioctl(fd, SPI_IOC_MESSAGE(2), &xfer);
	if (ret < 1){
		printf("can't xfer spi message1");
	}
#endif	
#if 1
readCmd[0] = 0xFF;
spiStateReg[0] = readAddress;
receiveData[0]=55;

memset(xfer, 0, 2*sizeof(struct spi_ioc_transfer));

xfer[0].tx_buf = (unsigned long)spiStateReg;
xfer[0].len = 1;


xfer[1].tx_buf = (unsigned long)readCmd;
xfer[1].rx_buf = (unsigned long)receiveData;
xfer[1].len = 1;

ret = ioctl(fd, SPI_IOC_MESSAGE(2), &xfer);
if (ret < 1){
//	printf("can't xfer spi message1");
}


#endif	




/*memset(xfer, 0, 2*sizeof(struct spi_ioc_transfer));

xfer[0].tx_buf = (unsigned long)spiStateReg;
xfer[0].len = 1;


//xfer[1].tx_buf = (unsigned long)spiStateReg;
xfer[1].rx_buf = (unsigned long)receiveData;
xfer[1].len = 1;

ret = ioctl(fd, SPI_IOC_MESSAGE(2), &xfer);
if (ret < 1){
	printf("can't xfer spi message1");
}
*/
/*
	xfer[0].tx_buf = (unsigned long)spiStateReg;
	xfer[0].rx_buf = (unsigned long)receiveData;
	xfer[0].len = 1;
	xfer[0].delay_usecs 	= 0;
	xfer[0].bits_per_word	= bits;   
	xfer[0].speed_hz		= speed;

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &xfer);
	if (ret < 1){
		printf("can't xfer spi message1\n");
	}
*/

	// printf("receiveData:%x ;\r\n",receiveData[0]);
	udata=receiveData[0];
	return receiveData[0];
}

int SPI_ReadBytes_LSB_1311(int fd, u8 spireg, u8 *buf, u8 length)
{
	int ret,i;
	u8 readCmd[128];
	u8 spiStateReg[1];
	u8 receiveData[8];
	struct spi_ioc_transfer xfer[3];
#if 0
	readCmd[0] = 0x03;
	spiStateReg[0] = spireg;
	memset(xfer, 0, 3*sizeof(struct spi_ioc_transfer));

	xfer[0].tx_buf = (unsigned long)readCmd;
	xfer[0].len = 1;

	xfer[1].tx_buf = (unsigned long)spiStateReg;
	xfer[1].len = 1;

	xfer[2].rx_buf = (unsigned long)buf;
	xfer[2].len = length;

	ret = ioctl(fd, SPI_IOC_MESSAGE(3), &xfer);
	if (ret < 1)
		printf("can't xfer spi message1");
#endif
#if 1
	for(i=0;i<length;i++){
		readCmd[i] = 0xFF;
	}
	spiStateReg[0] = spireg;
	receiveData[0]=55;
	
	memset(xfer, 0, 2*sizeof(struct spi_ioc_transfer));
	
	xfer[0].tx_buf = (unsigned long)spiStateReg;
	xfer[0].len = 1;
	
	
	xfer[1].tx_buf = (unsigned long)readCmd;
	xfer[1].rx_buf = (unsigned long)buf;
	xfer[1].len =length;
	
	ret = ioctl(fd, SPI_IOC_MESSAGE(2), &xfer);
	if (ret < 1){
//		printf("can't xfer spi message1");
	}
	
	
#endif	


/*	for(i = length; i>0; i--)
	{
		printf(" rxbuf[%d] = 0x %02x \r\n", (i-1), buf[i-1]);
	}*/

	return OK;
}






static void print_usage(const char *prog)
{
	printf("Usage: %s [-D]\n", prog);
	printf("  -D --device   device to use (default /dev/spidev1.1)\n");
	exit(1);
}
static int spidev_usage(void)
{
printf ("Usage : ./spi cs get  <reg> <size>\n"
"        ./spi cs set  <reg> <data>\n");
return 0;
}
unsigned int axtoi(char *str)
{
unsigned int result = 0;
char *p =NULL;
if(str == NULL)
{
	return 0;
}else if(str[0] == 'x' || str[0] == 'X'){
	p = str+1;
}else if(str[0] == '0'){
	if(str[1] == 'x' || str[1] == 'X')
	{
		p = str+2;
	}else{
		p = str;
	}
}else{
	p = str;
}


while(*p != '\0')
{
if('0' <= *p && *p <= '9')
result = result * 16 + (*p - 0x30);
else
if('a' <= *p && *p <= 'f')
result = result * 16 + (*p - 0x61 + 10);
else
if('A' <= *p && *p <= 'F')
result = result * 16 + (*p - 0x41 + 10);
else
return result;
p++;
}


return result;
}
static void sigint_handler(int dunno)
{
        switch (dunno) {
        case SIGINT:
                esc = true;
                printf("< Ctrl+C > Press.\n");
                break;
        default:
                break;
        }
}

#define GPIO_CMD_MAGE 'G'
#define GPIO_PWM0_SET     _IOW(GPIO_CMD_MAGE,0,char)
#define GPIO_PWM1_SET     _IOW(GPIO_CMD_MAGE,1,char)
#define GPIO_SET          _IOW(GPIO_CMD_MAGE,2,char)
#define BELL_GET         _IOW(GPIO_CMD_MAGE,3,char)
#define DET_GET          _IOW(GPIO_CMD_MAGE,4,char)
#define LED_PWM_SET      _IOW(GPIO_CMD_MAGE,5,char)
#define LED_PWM0_SET      _IOW(GPIO_CMD_MAGE,6,char)
#define RF_GET         _IOR(GPIO_CMD_MAGE,7,char)
#define CSN0_SET      _IOW(GPIO_CMD_MAGE,8,char)

void* w_handler(void* buf)
{
	int i=0,j=0;
	printf("pthread w_handler\n");
	pthread_mutex_init(&lock,NULL);
	pthread_cond_init(&notempty,NULL);

	while (esc == false) {
		pthread_mutex_lock(&lock);
		pthread_cond_wait(&notempty, &lock) ;
		pthread_mutex_unlock(&lock);
		printf("get card %x %x %x %x\n",TagInfo.uCardId[0],TagInfo.uCardId[1],TagInfo.uCardId[2],TagInfo.uCardId[3]);
	}	
	return 0;
}
int IF_GetCardId(CardInfoType* buf)
{
	int i=0,j=0;
	//printf("pthread w_handler\n");

	//while (esc == false) 
	{
		pthread_mutex_lock(&lock);
		pthread_cond_wait(&notempty, &lock) ;
		pthread_mutex_unlock(&lock);
		printf("IF_GetCardId uCardIdLen = %x\n",TagInfo.uCardIdLen);
		printf("IF_GetCardId [0~3]=%x %x %x %x\n",TagInfo.uCardId[0],TagInfo.uCardId[1],TagInfo.uCardId[2],TagInfo.uCardId[3]);
		printf("IF_GetCardId [4~7]=%x %x %x %x\n",TagInfo.uCardId[4],TagInfo.uCardId[5],TagInfo.uCardId[6],TagInfo.uCardId[7]);
		buf->uCardId[0]=TagInfo.uCardId[0];
		buf->uCardId[1]=TagInfo.uCardId[1];
		buf->uCardId[2]=TagInfo.uCardId[2];
		buf->uCardId[3]=TagInfo.uCardId[3];
		buf->uCardId[4]=TagInfo.uCardId[4];
		buf->uCardId[5]=TagInfo.uCardId[5];
		buf->uCardId[6]=TagInfo.uCardId[6];
		buf->uCardId[7]=TagInfo.uCardId[7];
		buf->TagType=TagInfo.TagType;
	}	
	return 0;
}

void* r_handler(void* buf)
{
	int i=0,j=0;
//sky1311Init(gpSky1311t);	


//cardTest(gpSky1311t);
//return 0;
	while (esc == false) {
		usleep(100000);
		Sky1311tHandleLoop();
		/*if(i){
			ioctl(pgpiofd,CSN0_SET,&i);
			i=0;
		}else{
			ioctl(pgpiofd,CSN0_SET,&i);
			i=1;
		}*/
		if(getcardid==1){
			getcardid=0;
			pthread_mutex_lock(&lock);
			pthread_cond_signal(&notempty);
			pthread_mutex_unlock(&lock);
			printf("pthread_cond_signal\n");
		}
		
	}	
	return 0;
}


//int main(int argc, char *argv[])
int IF_InitSPICard(void)
{
	int ret = 0;
	 u8 mode;
	 u8 bits = 8;
	 u32 speed = 50000;
	 u16 delay;
	 u8 lsb;

	 pthread_mutex_init(&lock,NULL);
	 pthread_cond_init(&notempty,NULL);

	 pgpiofd = open("/dev/mypwm", O_RDWR);
	 if(pgpiofd<0){
		 printf("can't open gpio err\n");
	 }
	 int j=1;
	 ioctl(pgpiofd,CSN0_SET,&j);


    spifd_1311 = open(spiDevice0, O_RDWR);
    if(spifd_1311 < 0){
        printf("can't open device");
    }

	//SPI_CPHA		 SPI_CPOL		
//    mode = mode|SPI_LSB_FIRST ;
    mode = 0;
//    printf("-- vis m %x \n",mode);
    /*
     * spi mode
     */
    ret = ioctl(spifd_1311, SPI_IOC_WR_MODE, &mode);
    if(ret == -1)
        printf("can't set spi mode");
 
    ret = ioctl(spifd_1311, SPI_IOC_RD_MODE, &mode);
    if(ret == -1)
        printf("can't get spi mode");
 
    /*
     * bits per word
     */
    ret = ioctl(spifd_1311, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if(ret == -1)
        printf("can't set bits per word");
 
    ret = ioctl(spifd_1311, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if(ret == -1)
        printf("can't get bits per word");
 
    /*
     * max speed hz
     */
    ret = ioctl(spifd_1311, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if(ret == -1)
        printf("can't set max speed hz");
 
    ret = ioctl(spifd_1311, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if(ret == -1)
        printf("can't get max speed hz");

    ret = ioctl(spifd_1311,SPI_IOC_RD_LSB_FIRST, &lsb);
    if(ret == -1)
        printf("can't get lsb first hz");
	
    printf("spi mode 1: %d\n", mode);
    printf("bits per word 2: %d\n", bits);
    printf("max speed: %d Hz (%d KHz)\n", speed, speed/1000);
	printf("lsb: %d\n", lsb);


	pthread_t thread_bell,thread_det;
//	ret = pthread_create(&thread_bell, NULL, w_handler, NULL);
//	if (ret) {
//			perror("[w_create] err");
//			return 1;
//	}
	
	 ret = pthread_create(&thread_det, NULL,r_handler, NULL);
	if (ret) {
			perror("[read_det] err ");
			return 1;
	}
	printf("pthread_create\n");

	return 0; //IF_InitSPICard return 

	signal(SIGINT, sigint_handler);
	pthread_join(thread_bell, NULL);
	pthread_join(thread_det, NULL);
	
	printf("done!\n");
	if(pgpiofd){
		close(pgpiofd);
	}
    if(spifd_1311){
        close(spifd_1311);
	}



out_close :
//close(fd);


return ret;
}


