#ifndef KEY_H
#define KEY_H
#include "sys.h"
#include "usart.h"
#define KEY0_PRES  4			//��������ֵ4
#define KEY1_PRES  1			//��������ֵ1
#define KEY2_PRES  2			//��������ֵ2
#define KEY3_PRES  3			//��������ֵ3
#define KEY_PRESUP 5
#define key0 PAin(0)
#define key1 PEin(4)
#define key2 PEin(3)
#define key3 PEin(2)
void key_init(void);			//��ʼ��4������
u8 key_scan(u8 mode);		//ɨ�谴��
//#define EN_INTERRUPT_KEY

extern u16 KeyTime;

#endif



