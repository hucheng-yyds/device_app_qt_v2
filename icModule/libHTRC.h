#ifndef __LIB_HTRC_H__
#define __LIB_HTRC_H__

#include <stdint.h>

//卡类型
#define CARD_TYPE_NONE          0xFF
#define CARD_TYPE_ISO7816       0x00
#define CARD_TYPE_CPU           0x01
#define CARD_TYPE_RFID          0x02
#define CARD_TYPE_MIFARE        0x03
#define CARD_TYPE_NTAG          0x04
#define CARD_TYPE_IDCARD        0x05
#define CARD_TYPE_SZT           0x11

//非接触卡返回的状态字
#define ST_OK                 0x9000  //正确执行
#define ST_ERROR              0xEEEE  //通用错误

#define ST_HARDPOWERDOWN_IN   0xA001  //正确执行
#define ST_HARDPOWERDOWN_OUT  0xA002  //正确执行
#define ST_SOFTPOWERDOWN_OUT  0xA003  //正确执行
#define ST_SOFTPOWERDOWN_IN   0xA004  //正确执行

#define ST_ERR_SOFTRESET      0xE001  //FM17550软复位错误 
#define ST_ERR_REQ            0xE002  //寻卡成功, 但卡的UID返回错误(UID返回为0xFF)
#define ST_ERR_REQLEN         0xE003  //Request()寻卡返回数据长度错误
#define ST_ERR_WAKELEN        0xE004  //WakeUp()寻卡返回数据长度错误
#define ST_ERR_COLLLEN        0xE005  //防冲突返回数据长度错误
#define ST_ERR_COLLBCC        0xE006  //防冲突返回置BCC校验错误
#define ST_ERR_SELLEN         0xE007  //选卡返回数据长度错误
#define ST_ERR_CARDTYPE       0xE008  //卡片类型错误

//CPU卡返回错误状态字
#define ST_ATSERR             0xE0A0  //ATS错误
#define ST_ATSERR_FWI         0xE0A1  //ATS错误, FWI值错误
#define ST_ATSERR_SFGI        0xE0A2  //ATS错误, SFGI值错误
#define ST_ATSERR_TC1_NAD     0xE0A3  //ATS错误, TC1的NAD位错误
#define ST_ATSERR_TC1_CID     0xE0A4  //ATS错误, TC1的CID位错误

#define ST_ERR_COMMAND        0xE101  //向FM17550发送命令数据的长度错误
#define ST_UNKNOWN_COMMAND    0xE102  //FM17550不识别的指令
#define ST_ERR_CLEARFIFO      0xE103  //FM17550清空FIFO错误
#define ST_PPSERR             0xE110  //PPS错误
#define ST_PPSERR_PARA        0xE120  //PICC不支持PPS指令的参数

#define ST_TIMEOUT            0xE1EE  //PCD或PICC超时TimeOut错误
#define ST_ERRREG             0xE200  //低四位为FM17550中ERRREG的值, 返回上一条指令的错误状态

#define ST_APDUERR            0xE301  //APDU指令返回错误
#define ST_APDUERR_COM        0xE302  //向卡发送错误的APDU命令
#define ST_APDUERR_HANDLE     0xE303  //Error Handling超过重试次数
#define ST_APDUERR_LINK       0xE304  //帧链接出错
#define ST_APDUERR_LEN        0xE305  //数据长度出错

#define ST_POWERR_LESS        0xE401  //Insufficient power for full functionality.
#define ST_POWERR_MORE        0xE402  //More than sufficient power for full functionality.

#define ST_ERR_TYPE           0xE501  //Card Type Error.
#define ST_ERR_VERIFY         0xE502  //Verify Card Error.
#define ST_ERR_READ           0xE503  //Read Card Error.

//M1卡返回错误状态字。
#define ST_ERR_MODE           0xE601   //Mifare_Authent()Mode输入参数错误
#define ST_ERR_ENCRYPTION     0xE602   //Mifare_Authent()加密标志位错误
#define ST_ERR_READLENGTH     0xE603   //Mifare_BlockRead()返回数据长度错误
#define ST_ERR_WRITEACK       0xE604   //Mifare_BlockWrite()返回ACK握手错误
#define ST_ERR_INCACK         0xE605   //Mifare_BlockInc()返回ACK握手错误
#define ST_ERR_DECACK         0xE606   //Mifare_BlockDec()返回ACK握手错误
#define ST_ERR_TRANSFERACK    0xE607   //Mifare_Transfer()返回ACK握手错误
#define ST_ERR_RESTOREACK     0xE608   //Mifare_Restore()返回ACK握手错误

//TYPEB卡返回错误状态字。
#define ST_ERR_TYPEBFWI       0xE701   //TypeB_Request()B卡ATQB返回FWI错误
#define ST_ERR_CIDVALUE       0xE702   //TypeB_Select()中卡片返回CID值错误

//Socket相关错误
#define ST_ERR_NET            0xE801   //网络错误
#define ST_ERR_NET_READ       0xE892   //网络读取错误
#define ST_ERR_NET_DATA       0xE893   //网络数据错误
#define ST_ERR_PERFORM        0xE894   //CURL请求错误
#define ST_LOGIN_DENIED       0xE895   //CURL认证错误

//==========文件IO错误==========
//SPI错误
#define ST_ERR_IO_CTRL        0xE901   //ioctrl错误
#define ST_ERR_FILE_OPEN      0xE902   //文件打开错误
#define ST_ERR_FILE_READ      0xE903   //文件读取错误

//USB错误
#define ST_ERR_SET_PARAMS     0xE904   //参数设置失败
#define ST_ERR_INIT_THREAD    0xE905   //初始化线程失败
#define ST_ERR_SEND_COMMAND   0xE906   //发送命令失败

//==========协议错误==========
#define ST_ERR_TRANS_TIMEOUT  0xEA01   //数据透传Timeout
#define ST_ERR_TRANS_PROTO    0xEA02   //数据透传协议错误
#define ST_ERR_TRANS_COMMAND  0xEA03   //数据透传命令错误
#define ST_ERR_TRANS_LEN      0xEA04   //数据透传长度错误

typedef struct tagCardInfo CardInfo;
struct tagCardInfo
{
    uint8_t CardType;
    uint8_t CardNumbLen;
    uint8_t CardNumb[16];
};

typedef struct tagIDCardInfo IDCardInfo;
struct tagIDCardInfo
{
    char    uid[24];            // 物理Id
    char    name[32];           // 姓名
    char    sex[8];             // 性别
    char    nation[64];         // 民族
    char    Birthday[20];       // 出生日期
    char    Address[120];       // 住址
    char    IdNumber[40];       // 身份证号
    char    Authority[32];      // 发证机关
    char    PeriodBeg[20];      // 有效开始日期
    char    PeriodEnd[20];      // 有效截止日期
    char    Reserved[40];       // 保留
    uint8_t WltPhoto[1024];     // WLT照片
    uint32_t BitmapLen;         // BMP照片长度
    uint8_t BmpPhoto[1024 * 50];// BMP照片
    //外国人
    char    enname[128];        // 英文姓名
    char    DeptCode[12];       // 受理机关编码
    char    location[8];        // 原籍地区编码
    char    certversion[8];     // 证件版本号
    char    certtype[4];        // 证件类型"I"是外国人，"J"是港澳台
    //港澳台
    char    passportNo[24];     // 通行证号码
    char    checkTimes[8];      // 签发次数
};

#ifdef __cplusplus
extern "C" {
#endif

uint16_t QueryCard(CardInfo *Ptr);
uint16_t Transmit(uint8_t *SendBuff, uint16_t SendLen,
                  uint8_t *RecvBuff, uint16_t *RecvLen);

uint16_t ReadIDCard(const char *UserName, const char *Password, const char *ProjData,
                    const char *ServHost, int ServPort, IDCardInfo *Ptr);

#if defined(DIRECT_FM175XX)
extern void FM175xxReset(void);
#endif

#define USE_INTERN_SPI
#if defined(USE_INTERN_COM) || defined(USE_INTERN_SPI) || defined(USE_INTERN_USB)
uint16_t OpenReader(void);
void CloseReader(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
