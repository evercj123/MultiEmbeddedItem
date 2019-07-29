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
void led(LED_e ledn,u8 statu);//ledn 第几个led ；statu led状态 1亮 0灭 
void led_turn(LED_e ledn);    //turn the led statu
void gpio_d_init(void);       //初始化gpio D8
#endif
