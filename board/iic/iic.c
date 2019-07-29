#include<iic.h>
#include"delay.h"
#include"usart.h"
#include"sys.h"
void IIC_Init(void)
{
	RCC->APB2ENR|=1<<3;
	GPIOB->CRH&=0XFFFF00FF;
	GPIOB->CRH|=0X00003300;
	GPIOB->ODR|=3<<10;		//OUTPUT HIGHT
   
}
void IIC_Start(void)
{

	SDA_OUT();
	IIC_SDA=1;
	IIC_SCL=1;
	delay_us(2);
	IIC_SDA=0;delay_us(4);
	IIC_SCL=0;

}

void IIC_Stop(void)
{
	SDA_OUT();
	IIC_SDA=0;
	IIC_SCL=0;delay_us(4);
	IIC_SCL=1;delay_us(4);
	IIC_SDA=1;
}
u8 IIC_Wait_ACK(void)
{
	u8 ErrTime=0;	
   //IIC_SCL=0;
	IIC_SDA=0;
	SDA_IN();
	delay_us(3);
	IIC_SCL=1;delay_us(1);
	while(READ_SDA==1)
	{
		ErrTime++;
		if(ErrTime>250)//low voltage pulse is the  stop signal 250  13us
		{
			IIC_Stop();
			return 1;
		}
	}
	return 0;
}
void IIC_ACK(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	IIC_SCL=0;delay_us(2);
	IIC_SCL=1;delay_us(2);
	IIC_SCL=0;
}
void IIC_NACK(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SCL=0;
	IIC_SDA=1;delay_us(2);
	IIC_SCL=1;delay_us(2);
	IIC_SCL=0;

}
void IIC_SendByte(u8 byte)
{
	u8 n;
	SDA_OUT();
	IIC_SCL=0;
	for(n=0;n<8;n++)
	{
		//
		IIC_SDA=(byte&0X80)>>7;
		byte=byte<<1;
		delay_us(2);
		IIC_SCL=1;delay_us(3);
		IIC_SCL=0;delay_us(1);
	}
}	

u8 IIC_ReadByte(unsigned char ack)
{
	u8 n,receive;
	SDA_IN();
	receive=0;
	for(n=0;n<8;n++)
	{	
		IIC_SCL=0;
		delay_us(3);
		IIC_SCL=1;
		delay_us(2);
		receive=receive<<1;		
		if(READ_SDA)
		{		
			receive++;
		}
		//IIC_SCL=0;   //×¢ÒâÀ­µÍ
		delay_us(1);	
	}
	if(!ack)IIC_NACK();
	else IIC_ACK();
	return receive;
}



