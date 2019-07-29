#ifndef LED_H
#define LED_H
#include "sys.h"
#include "usart.h"
#include "delay.h"
//#include  "key.h"

typedef enum
{
   LED0,
	LED1,
	LED_MAX,
}LED_e;
void led_init(void);
void led(LED_e ledn,u8 statu);//ledn �ڼ���led ��statu led״̬ 1�� 0�� 
void led_turn(LED_e ledn);    //turn the led statu
void gpio_d_init(void);       //��ʼ��gpio D8
#endif
