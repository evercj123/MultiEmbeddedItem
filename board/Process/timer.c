#include "timer.h"
#include "sys.h"
#include "process.h"
//time not beyound 655 ms
void timer3_init(u16 time,u16 psc)
{
   u32 time_val;
	time_val=time*10-1;
	RCC->APB1ENR |=1<<1;
	TIM3->ARR=time_val;			//��ʱֵ 
	TIM3->PSC =psc; 			//Ԥ��Ƶֵ������ʱ��Ϊ72Mhz��psc Ϊ72ʱ ������ʱ������Ϊ 1/1000000s �� 1us
	TIM3->DIER|=1<<0;			//ALLOWE UP INTERRUPT
	TIM3->CR1|=0X01;			//ENABLE BEGIN TIME
	MY_NVIC_Init(1,3,TIM3_IRQn,2); 
}
void timer3_close(void)
{
	TIM3->CR1&=~0X01;
}
void TIM3_IRQHandler(void)
{
   if(TIM3->SR&0x0001)
	{
			TaskProcess();
	}
	TIM3->SR&=~0X0001;
}