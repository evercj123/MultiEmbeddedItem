#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <system_stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_exti.h>
#include <misc.h>
#include <stdio.h>
#include "lwip_demo.h"

extern void SPI1_Init(void);
int gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
                         RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	return 0;
}

int nvic_configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);



	return 0;
}

int Spi_InitEnc28j60(void)
{


	return 0;
}
int main_init(void)
{
	//时钟设置
	SystemInit();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	gpio_init();
	SPI1_Init();
	nvic_configuration();
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;					  //配置外部中断1
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			  //中断模式
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;		  //下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);     //外部中断配置,网卡中断
	return 0;
}


int main()
{
	main_init();
	lwip_demo();
	while(1);
}