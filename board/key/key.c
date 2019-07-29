#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "key.h"

void key_init(void)
{
	RCC->APB2ENR|=1<<2;
	RCC->APB2ENR|=1<<6;
   GPIOA->CRL&=0XFFFFFFF0;      //°´¼ükey0
	GPIOA->CRL|=0X08;
	GPIOE->CRL&=~0x000fff00;      //°´¼ükey1 key2 key3
   GPIOE->CRL|=0x00088800;
	GPIOE->ODR|=0X07<<2;
	

}
u8 Keyf_5ms;
u16 Keycnt;
u8 KeyDown;
u16 KeyTime;
u8 key_scan(u8 mode)
{	
	if(key1==0)
	{
			KeyDown=1;
			Keycnt++;

	}
	else 
	{		if(KeyDown==1)
			{
					KeyDown=0;
					if(Keycnt>10)
					{
								
								KeyTime=Keycnt;
					}
						
			}
			Keycnt=0;
	
	}
	
	
	return 0;
}









