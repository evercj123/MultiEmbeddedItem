#ifndef _ILI9325_H_
#define _ILI9325_H_
#include "stm32f10x.h"
#include "delay.h"
#define	LCD_CS_SET  GPIOG->BSRR|=1<<12   //片选端口   	PC9
#define	LCD_RS_SET	GPIOG->BSRR|=1<<0    //数据/命令 	PC8	   
#define	LCD_WR_SET	GPIOD->BSRR|=1<<5    //写数据			PC7
#define	LCD_RD_SET	GPIOD->BSRR|=1<<4    //读数据			PC6
								    
#define	LCD_CS_CLR  GPIOG->BRR|=1<<12   //片选端口  	   PC9
#define	LCD_RS_CLR	GPIOG->BRR|=1<<0     //数据/命令		PC8	   
#define	LCD_WR_CLR	GPIOD->BRR|=1<<5     //写数据			PC7
#define	LCD_RD_CLR	GPIOD->BRR|=1<<4     //读数据			PC6   


#define LCD_WR_DATA(data){\
LCD_RS_SET;\
LCD_CS_CLR;\
DATAOUT(data);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
} 
void Lcdinit(void);
void writeReg(uint16_t regnum,uint16_t regvalue);
uint16_t readReg(uint8_t data);

#endif

