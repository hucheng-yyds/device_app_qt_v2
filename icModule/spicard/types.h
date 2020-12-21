
#ifndef TYPES_H
#define TYPES_H

//#include "Config.h"
//#include "types.h"

//#define USE_DEBUG 1
//#define USE_DEBUG_TX_ENABLE 1

#define xdata
#define data
#define code const

#define NOP() __NOP()

typedef unsigned char		BYTE;
typedef signed char		SBYTE;
typedef unsigned char		uchar;
typedef unsigned char		HighLow;
typedef unsigned short		WORD;
typedef signed short		SWORD;
typedef unsigned long		DWORD;
typedef signed long		SDWORD;
typedef signed char		Schar;
typedef signed int		Sint;
typedef signed long		Slong;
typedef unsigned char		StringType;
#if 1//def ENABLE_64BIT_TYPE
typedef unsigned long long u64;
typedef signed long long	s64;
#endif

typedef unsigned long  const               uc32;				
typedef unsigned short const               uc16;				
typedef unsigned char  const               uc8;			

typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;

//typedef unsigned char  uint8_t;
//typedef unsigned short  uint16_t;
//typedef unsigned long  uint32_t;

typedef enum {FALSE = 0, TRUE = !FALSE} BOOL;

//#define BOOL				bool
//#define false				FALSE
//#define true				TRUE

//typedef void (*fpSetBoolValue)(BOOL);
//typedef BOOL (*fpGetBoolValue)(void);

typedef void (*fpSetU8Value)(BYTE);
typedef BYTE (*fpGetU8Value)(void);

typedef void (*fpSetS8Value)(SBYTE);
typedef SBYTE (*fpGetS8Value)(void);

typedef void (*fpSetU16Value)(WORD);
typedef WORD (*fpGetU16Value)(void);

typedef void (*fpSetS16Value)(SWORD);
typedef SWORD (*fpGetS16Value)(void);

typedef void (*fpSetU32Value)(DWORD);
typedef DWORD (*fpGetU32Value)(void);

typedef void (*fpSetS32Value)(SDWORD);
typedef SDWORD (*fpGetS32Value)(void);

#define BIT0		(0x00000001)
#define BIT1		(0x00000002)
#define BIT2		(0x00000004)
#define BIT3		(0x00000008)
#define BIT4		(0x00000010)
#define BIT5		(0x00000020)
#define BIT6		(0x00000040)
#define BIT7		(0x00000080)
#define BIT8		(0x00000100)
#define BIT9		(0x00000200)
#define BIT10	(0x00000400)
#define BIT11	(0x00000800)
#define BIT12	(0x00001000)
#define BIT13	(0x00002000)
#define BIT14	(0x00004000)
#define BIT15	(0x00008000)
#define BIT16	(0x00010000)
#define BIT17	(0x00020000)
#define BIT18	(0x00040000)
#define BIT19	(0x00080000)
#define BIT20	(0x00100000)
#define BIT21	(0x00200000)
#define BIT22	(0x00400000)
#define BIT23	(0x00800000)
#define BIT24	(0x01000000)
#define BIT25	(0x02000000)
#define BIT26	(0x04000000)
#define BIT27	(0x08000000)
#define BIT28	(0x10000000)
#define BIT29	(0x20000000)
#define BIT30	(0x40000000)
#define BIT31	(0x80000000)

#ifndef U8_MAX
#define U8_MAX     (255)
#endif
#ifndef S8_MAX
#define S8_MAX     (127)
#endif
#ifndef S8_MIN
#define S8_MIN     (-128)
#endif
#ifndef U16_MAX
#define U16_MAX    (65535u)
#endif
#ifndef S16_MAX
#define S16_MAX    (32767)
#endif
#ifndef S16_MIN
#define S16_MIN    (-32768)
#endif
#ifndef U32_MAX
#define U32_MAX    (4294967295uL)
#endif
#ifndef S32_MAX
#define S32_MAX    (2147483647)
#endif
#ifndef S32_MIN
#define S32_MIN    (-2147483648uL)
#endif

#define HIBYTE_REF(addr)				(*((BYTE *) & (addr)))
#define LOBYTE_REF(addr)			(*((BYTE *) & (addr + 1)))
#define MAKEWORD(value1, value2)	(((WORD)(value1)) * 0x100) + (value2)
#define MAKEUINT(value1, value2)		(((WORD)(value1)) * 0x100) + (value2)
#define H2BYTE(value)				((BYTE)((value) / 0x10000))
#define HIBYTE(value)				((BYTE)((value) / 0x100))
#define LOBYTE(value)				((BYTE)(value))

#define POWER2(bits)					((DWORD)BIT0<<bits)
#define BitMask(bits)					((DWORD)BIT0<<bits)
#define MAKE_MASK(bits)				(POWER2(bits)-1)

#define ANY_VALUE					0

#define _LOW						0
#define _HIGH						1

#define RET_FAILURE		FALSE
#define RET_ERROR		FALSE
#define RET_SUCCESS		TRUE

#define ON							TRUE
#define OFF							FALSE

#define DIFFERENT					TRUE
#define SAME							FALSE

#define ST_LOCK						TRUE
#define ST_UNLOCK					FALSE

typedef union{
	DWORD dwValue;
	SDWORD sdwValue;
	WORD wValue;
	SWORD swValue;
	BYTE ucValue;
	SBYTE sbValue;
	//BOOL bValue;
	struct {
		DWORD Bit31:1;
		DWORD Bit30:1;
		DWORD Bit29:1;
		DWORD Bit28:1;
		DWORD Bit27:1;
		DWORD Bit26:1;
		DWORD Bit25:1;
		DWORD Bit24:1;
		DWORD Bit23:1;
		DWORD Bit22:1;
		DWORD Bit21:1;
		DWORD Bit20:1;
		DWORD Bit19:1;
		DWORD Bit18:1;
		DWORD Bit17:1;
		DWORD Bit16:1;
		DWORD Bit15:1;
		DWORD Bit14:1;
		DWORD Bit13:1;
		DWORD Bit12:1;
		DWORD Bit11:1;
		DWORD Bit10:1;
		DWORD Bit9:1;
		DWORD Bit8:1;
		DWORD Bit7:1;
		DWORD Bit6:1;
		DWORD Bit5:1;
		DWORD Bit4:1;
		DWORD Bit3:1;
		DWORD Bit2:1;
		DWORD Bit1:1;
		DWORD Bit0:1;
	} B;
}TmpType;

typedef enum{
	ET_LITTLE_END= 0,
	ET_BIG_END= 1
}EndianType;

typedef enum{
	ST_LOW= 0,
	ST_HIGH= 1
}HiLowType;

typedef enum{
	EDGE_FALL= 0,
	EDGE_RISE= 1
}EdgeType;

typedef enum{
	PIN_LOW= 0,
	PIN_HIGH= 1,
	PIN_FLOAT= 2
}PinStateType;

typedef enum{
	ST_CLOSE= 0,
	ST_OPEN= 1
}OpenCloseType;

typedef enum{
	ST_OFF= 0,
	ST_ON= 1
}OnOffType;

#ifndef NULL
#define NULL		(void*)(0)
#endif



#endif

