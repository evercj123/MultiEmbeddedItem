#include<stm32f10x.h>
#include"sys.h"
#include"delay.h"
//#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}
#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}
#define IIC_SCL   PBout(10)
#define IIC_SDA   PBout(11)
#define READ_SDA  PBin(11)

void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
u8 IIC_Wait_ACK(void);
u8 IIC_ReadByte(unsigned char ack);
void IIC_SendByte(u8 byte);