#include "led.h"
//初始化led
void led_init(void)
{
	 RCC->APB2ENR|=1<<3;    
    GPIOB->CRL&=0XFF0FFFFF; 
	 GPIOB->CRL|=0X00300000;   	 
    GPIOB->ODR|=1<<5;   
}

//statu 1亮 0灭
//LED_e led0 led1
void led(LED_e ledn,u8 statu)
{
  if(ledn==LED0)
  {
	  if(statu==0)
			PBout(5)=0;
	  else if(statu==1)
			PBout(5)=1;
  }else if(ledn==1)
  {
  
  }else if(ledn==2)
  {
	if(statu==0)
     PBout(5)=0;
	else 
		PBout(5)=1;
  }
  
}
//翻转led的状态
void led_turn(LED_e ledn)
{
	if(ledn==LED0)
	{
		if(GPIOB->ODR&0x00000020)
			GPIOB->ODR&=~0X00000020;
		else PBout(5)=1;
	}
}
void gpio_d_init(void)
{
	 RCC->APB2ENR|=1<<5;    
    GPIOD->CRH&=0XFFFFFFF0; 
	 GPIOD->CRH|=0X00000003;   	 
    GPIOD->ODR|=1<<0;   
}
