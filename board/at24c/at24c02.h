#include<stm32f10x.h>
#include"iic.h"
#include"delay.h"

#define  AT24C02 256
#define  AT24C16 1024
#define  EE_TYPE AT24C02
void AT24CxxInit(void);
u8   AT24CReadByte(u8 addr);
void AT24CWriteByte(u8 addr,u8 dataWrite);
void AT24CWriteByteLen(u8 addr,u8 * data,u8 len);
void AT24CReadByteLen(u8 addr,u8 *data,u8 len);
u8	  AT24CCheck(void);