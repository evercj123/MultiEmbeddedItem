#include "ili9325.h"
#include "sys.h"
typedef enum 
{
	D0=0,D1,D2,D3,D4,D5,D6,D7,D8,D9,D10,D11,D12,D13,D14,D15,
	  E0,E1,E2,E3,E4,E5,E6,E7,E8,E9,E10,E11,E12,E13,E14,E15
}LcdPin;
const LcdPin LcdDataPin[16]={D14,D15,D0,D1,E7,E8,E9,E10,E11,E12,E13,E14,E15,D8,D9,D10};

void Lcdinit(void)
{
	RCC->APB2ENR|=1<<3;//先使能外设PORTB时钟
 	RCC->APB2ENR|=1<<4;//先使能外设PORTC时钟
	RCC->APB2ENR|=1<<5;
	RCC->APB2ENR|=1<<6;
	RCC->APB2ENR|=1<<7;
	RCC->APB2ENR|=1<<8;
	RCC->APB2ENR|=1<<0;    //开启辅助时钟
	
	GPIOG->CRH&=0XFFF0FFFF;//g12
	GPIOG->CRH|=0X00030000; 
	GPIOG->CRL&=0XFFFFFFF0;//G0
	GPIOG->CRL|=0X00000003; 
	GPIOD->CRL&=0XFF00FFFF;//D4 D5
	GPIOD->CRL|=0X00330000;  
	GPIOD->ODR|=0X0030; 
	GPIOG->ODR|=0X1001;
}
void Lcdpingout(void)
{	
	uint8_t n;
	for(n=0;n<16;n++)
	{
		if(LcdDataPin[n]<16)
		{
			if(LcdDataPin[n]<8)
			{
				GPIOD->CRL&=(uint32_t)(~(uint32_t)(0Xf<<LcdDataPin[n]*4));
				GPIOD->CRL|=(0X3<<LcdDataPin[n]*4); //上拉输出
				GPIOD->BSRR|=1<<LcdDataPin[n];
				//PDout(LcdDataPin[n])=1;
			}
			else
			{
				GPIOD->CRH&=(uint32_t)(~(uint32_t)(0Xf<<(LcdDataPin[n]-8)*4));
				GPIOD->CRH|=(0X3<<(LcdDataPin[n]-8)*4);  
				GPIOD->BSRR|=1<<LcdDataPin[n];
				//PDout(LcdDataPin[n])=1;
			}
			
		}
		else
		{
			if((LcdDataPin[n]-16)<8)
			{
				GPIOE->CRL&=(uint32_t)(~(uint32_t)(0Xf<<(LcdDataPin[n]-16)*4));
				GPIOE->CRL|=(0X3<<(LcdDataPin[n]-16)*4); //上拉输出
				GPIOE->BSRR|=(1<<(LcdDataPin[n]-16));
				//PEout(LcdDataPin[n]-16)=1;
			}
			else
			{
				GPIOE->CRH&=(uint32_t)(~(uint32_t)(0Xf<<(LcdDataPin[n]-16-8)*4));
				GPIOE->CRH|=(0X3<<(LcdDataPin[n]-16-8)*4);
				GPIOE->BSRR|=(1<<(LcdDataPin[n]-16));
				//PEout(LcdDataPin[n]-16)=1;
			}
			
		
		}
	}

}
void Lcdpingin(void)
{
	uint8_t n;
	for(n=0;n<16;n++)
	{
		if(LcdDataPin[n]<16)
		{
			if(LcdDataPin[n]<8)
			{
				GPIOD->CRL&=(uint32_t)(~(uint32_t)(0Xf<<LcdDataPin[n]*4));
				GPIOD->CRL|=(0X8<<LcdDataPin[n]*4); 
				//GPIOD->ODR&=(uint32_t)(~(1<<LcdDataPin[n]));
				//GPIOD->BRR|=1<<LcdDataPin[n];
			}
			else
			{
				GPIOD->CRH&=(uint32_t)(~(uint32_t)(0Xf<<(LcdDataPin[n]-8)*4));
				GPIOD->CRH|=(0X8<<(LcdDataPin[n]-8)*4);
				//GPIOD->BRR|=1<<LcdDataPin[n];
			}
			
		}
		else
		{
			if((LcdDataPin[n]-16)<8)
			{
				GPIOE->CRL&=(uint32_t)(~(uint32_t)(0Xf<<(LcdDataPin[n]-16)*4));
				GPIOE->CRL|=(0X8<<(LcdDataPin[n]-16)); 
				//GPIOE->BRR|=1<<(LcdDataPin[n]-16);
			}
			else
			{
				GPIOE->CRH&=(uint32_t)(~(uint32_t)(0Xf<<(LcdDataPin[n]-16)));
				GPIOE->CRH|=(0X8<<(LcdDataPin[n]-16-8)*4);
				//GPIOE->BRR|=1<<(LcdDataPin[n]-16);
			}		 		
		}
	}
}

void DATAOUT(uint16_t data)
{
	uint8_t n;
	for(n=0;n<16;n++)
	{
		if(LcdDataPin[n]<16)
		{
//			GPIOD->ODR&=(uint32_t)(~(uint32_t)(0X1<<LcdDataPin[n]));
//			GPIOD->ODR|=(0x1<<LcdDataPin[n]);
			if((data&0x0001)!=0)
				PDout(LcdDataPin[n])=1;
			else 
				PDout(LcdDataPin[n])=0;
		}
		else
		{
//			GPIOE->ODR&=(uint32_t)(~(uint32_t)(0X1<<(LcdDataPin[n]-16)));
//			GPIOE->ODR|=(0x1<<(LcdDataPin[n]-16));
			if((data&0x0001)!=0)
				PEout(LcdDataPin[n]-16)=1;
			else 
				PEout(LcdDataPin[n]-16)=0;
		}
		data>>=1;
	}
}

uint16_t DATAIN(void)
{
	uint16_t data;
	int8_t n;
	data=0;
	for(n=15;n>=0;n--)
	{

		if(LcdDataPin[n]<16)
		{
			data<<=1;
//			data|=PDin(LcdDataPin[n]);
			if(PDin(LcdDataPin[n])!=0)
				data++;
			else ;
			
		}
		else
		{	
			data<<=1;
			//data|=PEin(LcdDataPin[n]-16);
			if(PEin(LcdDataPin[n]-16)!=0)
				data++;
			else ;
		
		}		
	}
	return data;
}

void LCD_WR_REG(u16 data)
{ 
	LCD_RS_CLR;//写地址 
 	LCD_CS_CLR; 
	
	LCD_WR_CLR;
	LCD_RD_SET;
	DATAOUT(data);  
	
	LCD_WR_SET; 
 	LCD_CS_SET;   
}

void LCD_WR_DATAX(u16 data)
{
	LCD_RS_SET;
	LCD_CS_CLR;
	LCD_WR_CLR;
	DATAOUT(data);

	LCD_WR_SET;
	LCD_CS_SET;
}
u16 LCD_RD_DATA(void)
{										   
	u16 t;
	Lcdpingin();
	LCD_RS_SET;
	LCD_WR_SET;
	LCD_CS_CLR;//读取数据(读寄存器时,并不需要读2次)
	LCD_RD_CLR;					   
	t=DATAIN();  
	LCD_RD_SET;
	LCD_CS_SET; 

	Lcdpingout();

	return t;  
}
void writeReg(uint16_t regnum,uint16_t regvalue)
{
	LCD_WR_REG(regnum);
	LCD_WR_DATAX(regvalue);
	
}
uint16_t readReg(uint8_t data)
{
	LCD_WR_REG(data);  //写入要读的寄存器号  
	return LCD_RD_DATA(); 

}
