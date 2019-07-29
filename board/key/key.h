#ifndef KEY_H
#define KEY_H
#include "sys.h"
#include "usart.h"
#define KEY0_PRES  4			//按键返回值4
#define KEY1_PRES  1			//按键返回值1
#define KEY2_PRES  2			//按键返回值2
#define KEY3_PRES  3			//按键返回值3
#define KEY_PRESUP 5
#define key0 PAin(0)
#define key1 PEin(4)
#define key2 PEin(3)
#define key3 PEin(2)
void key_init(void);			//初始化4个按键
u8 key_scan(u8 mode);		//扫描按键
//#define EN_INTERRUPT_KEY

extern u16 KeyTime;

#endif



