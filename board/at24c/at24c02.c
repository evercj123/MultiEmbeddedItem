#include"at24c02.h"
#include"usart.h"
void AT24CxxInit(void)
{
	IIC_Init();
}
//address  data
u8 AT24CReadByte(u8 addr)
{
	u8 temp;
	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_SendByte(0XA0);
		IIC_Wait_ACK();		
		IIC_SendByte(addr>>8);		
	}
	else 
	{
		IIC_SendByte(0xA0+((addr/256)<<1));
	}
	IIC_Wait_ACK();//写命令ok?
	IIC_SendByte(addr%256);
//	IIC_SendByte(0);
	IIC_Wait_ACK();//写读地址ok?
	IIC_Start();
	IIC_SendByte(0xa1);//发送设备地址
	IIC_Wait_ACK();//
	temp=IIC_ReadByte(0);//读数据
	IIC_Stop();
	return temp;
}
//address  data
void AT24CWriteByte(u8 addr,u8 dataWrite)
{

	IIC_Start();
	if(EE_TYPE>AT24C16)
	{
		IIC_SendByte(0XA0);
		IIC_Wait_ACK();
		IIC_SendByte(addr>>8);
	}
	else 
	{
		IIC_SendByte(0xA0+((addr/256)<<1));
	
	}
	IIC_Wait_ACK();
	IIC_SendByte(addr%256);
	//IIC_SendByte(0);
	IIC_Wait_ACK();
	IIC_SendByte((u8)dataWrite);
	IIC_Wait_ACK();	
	IIC_Stop();
	delay_ms(10);
	return ;
}
void AT24CWriteByteLen(u8 addr,u8 *data,u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		AT24CWriteByte(addr+i,*(data+i));
	}

}
void AT24CReadByteLen(u8 addr,u8 *data,u8 len)
{
	u8 i;
	for(i=0;i<len;i++)
	{
		*(data+i)=AT24CReadByte(addr+i);
	}
}
//0 success 1 fail
u8 AT24CCheck(void)
{	
	u8 temp;
	AT24CWriteByte(255,'x');
	temp=AT24CReadByte(255);
	printf("验证：%c",temp);
	if(temp==0x55)return 0;
	else
	{
		AT24CWriteByte(0,0x55);
	   temp=AT24CReadByte(0);
		if(temp==0x55)return 0;
	}
	return 1;
}
	







