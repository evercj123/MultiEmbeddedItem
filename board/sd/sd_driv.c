/*****************************************************************
*数据传输时bit位顺序
*SD模式下
*Usual data (8-bit width): The usual data (8-bit width) are sent in LSB (Least Significant Byte) first,
*MSB (Most Significant Byte) last sequence. But in the individual byte, it is MSB (Most Significant Bit)
*first, LSB (Least Significant Bit) last.
*Wide width data (SD Memory Register): The wide width data is shifted from the MSB bit
*
*SPI模式下
*****************************************************************/



#include "stm32f10x.h"
#include "sd_driv.h"
#include "spi.h"
#include "Head.h"
uint8_t  SD_Type;			//SD卡的类型
uint8_t  UERR;
void pin_init(void)
{

			RCC->APB2ENR|=1<<5;			//PD				
			GPIOD->CRL&=0XFFFFF0FF;
			GPIOD->CRL|=0X00000300;
			GPIOD->ODR|=0X1<<2; 
}
 

/*****************************************************************
*返回 0: OK    0xff:error
*
*****************************************************************/
 uint8_t SD_Waite_Ready(void)
 {
		u32 i;
		do
    {
        if(SPI1_ReadWriteByte(0xff) == 0xff)
        {    
            return 0;
        }
        i++;
      
		}while(i<0xffffff);

		return 0xff;
 }
 
 
 /*****************************************************************
 * SD卡失能
 *
 *****************************************************************/
 void SD_DisSel(void)
 {
    SD_CS=1;
    UERR = SPI1_ReadWriteByte(0xff);//读响应，8个时钟
 }
 
 
 /*****************************************************************
 *片选 低使能
 *写入指令，发送8个时钟
 *****************************************************************/ 
 uint8_t SD_Select(void)
 {
    SD_CS=0;
    if(SD_Waite_Ready() == 0)
    {
        return 0;
    }
    SD_DisSel();
    
		return 1;
				
 }
 
 
 
 /*****************************************************************
 *
 *
 *****************************************************************/
 uint8_t SendCmd(uint8_t cmd,u32 arg,uint8_t crc)
 {
    uint8_t retry;//,i;
    uint8_t err;
    SD_DisSel();
    if(SD_Select()!=0)
    {  
         return 0xff;
    }  


    SPI1_ReadWriteByte(cmd|0x40);
    SPI1_ReadWriteByte(arg>>24);
    SPI1_ReadWriteByte(arg>>16);
    SPI1_ReadWriteByte(arg>>8);
    SPI1_ReadWriteByte(arg);
    SPI1_ReadWriteByte(crc);

    if(cmd == CMD12)
    {
        SPI1_ReadWriteByte(0XFF);		//停止数据传输
    }
    retry=0x1f;	
    do
    {
        err = SPI1_ReadWriteByte(0xff); //等待起始响应 
        retry--;

    }while((err == 0xff)&&retry);	
    if(!retry) err = 0xff;
    return err;
 }
 

/*****************************************************************
 *
 *
 *****************************************************************/ 
 uint8_t RessetSD(void)
 {	
    uint8_t i;
    u16 retry;
    uint8_t response;
    SD_DisSel();
    for(i=0;i<8;i++)SPI1_ReadWriteByte(0xff);
    SD_CS=0;
    retry=0xfffe;
    do{
          response=SendCmd(CMD0,0,0x95);
          retry--;
    }while((response!=0x01)&&retry);
 

    SD_DisSel();
    return 0;  //返回成功
 }
 
 
 
 /*****************************************************************
 *
 *
 *****************************************************************/
 #if 0
 uint8_t Initialize_SD(void)
 {
    uint8_t i;
    uint8_t err;
    u16 retry;
    uint8_t buff[4];


    pin_init();    
    SPI1_Init(); 
    SD_DisSel();
    SPI1_SetSpeed(SPI_SPEED_256);
    
    //RessetSD();

    
    for(i = 0;i < 10;i++)
    {
        SPI1_ReadWriteByte(0xff);  //发送至少74个时钟
    }
    retry = 20;
    do
    {
        err = SendCmd(CMD0,0,0x95);   //进入空闲
    }while((err != 0x01) && retry--);
    SD_Type=0;
    if(err == 1)
    {
      if(SendCmd(CMD8,0x1AA,0x87)==1)   //0x000001AA
      {
          for(i = 0;i < 4;i++)
          {
               buff[i] = SPI1_ReadWriteByte(0xff);
          }
          if(buff[2] == 0X01 && buff[3] == 0XAA)//卡是否支持2.7~3.6V
          {
              retry = 0XFFFE;
              do
              {
                  SendCmd(CMD55,0,0X01);	//发送CMD55				
                  err = SendCmd(CMD41,0x40000000,0X01);//是否是高容量卡 0x40000000 HCS
              }while(err && retry--);

              if(retry && SendCmd(CMD58,0,0X01) == 0)//鉴别SD2.0卡版本开始(R3register)
              {
                  for(i = 0;i < 4;i++)
                  {     
                       buff[i] = SPI1_ReadWriteByte(0XFF);//得到OCR值
                  }
                  if(buff[0] & 0x40)  //bit30 card capacity status
                  {
                       SD_Type = SD_TYPE_V2HC;    //检查CCS 大容量卡
                  }
                  else 
                       SD_Type = SD_TYPE_V2;  
              }

          }

      }

    }
    else
    {
        SendCmd(CMD55,0,0X01);		//指定特殊命令
        err = SendCmd(CMD41,0,0X01);	//读取卡的类型 orc寄存器
        if(err <= 1)
        {		
            SD_Type = SD_TYPE_V1; 
            retry = 0XFFFE;
            do 
            {
                SendCmd(CMD55,0,0X01);	
                err=SendCmd(CMD41,0,0X01);
            }while(err && retry--);
        }
        else
        {
            SD_Type=SD_TYPE_MMC;//MMC V3
            retry=0XFFFE;
            do 
            {											    
                err = SendCmd(CMD1,0,0X01);
            }while(err && retry--);  
        }
        if(retry == 0 || SendCmd(CMD16,512,0X01) != 0)
        {           
            SD_Type = SD_TYPE_ERR;
        }

    }
    SD_DisSel();
    SPI1_SetSpeed(SPI_SPEED_2);
    
    if(SD_Type)
    {
         return 0;
    }
    else 
    {
         if(err)
         {
             return 0xff;
         }
    }
    return 0xff;					
}
 
#endif 
/*****************************************************************
*
*
*****************************************************************/
uint8_t Initialize_SD(void)
{
    int err = 0,cnt;
    pin_init();    
    SPI1_Init(); 
    SD_DisSel();
    SPI1_SetSpeed(SPI_SPEED_256);
    for(uint8_t i = 0; i < 10;i++)
    {
        SPI1_ReadWriteByte(0xff);
    }
    cnt = 200;
    do 
    {
         err = SendCmd(CMD0,0,0x95);   
    }while((err != 0xff ) && cnt--); //返回
    if(err == 0x01)//IDLE状态，OR初始化状态？
    {
         err = SendCmd(CMD8,0x1AA,0x87);
         if(err == 0X01)
         {
              uint8_t buf[4];
              for(uint8_t i = 0;i < 4;i++)
              {
                  buf[i] = SPI1_ReadWriteByte(0xff);
              }
//            if(buf[2]) //检查电压

              int retry = 200;
              do
              {   SendCmd(CMD55,0,0x01);
                  err = SendCmd(CMD41,0x40000000,0X01);//r1返回 1 :正在初始化中
              }while(err && retry--);
              if(!err)
              {
                  err = SendCmd(CMD58,0,0x01);
                  if(!err)
                  {
                      uint8_t ocr_buf[4];
                      for(uint8_t i = 0;i < 4;i++)
                      {
                          ocr_buf[i] = SPI1_ReadWriteByte(0xff);
                      }
                      if(ocr_buf[0] & 0x40)  
                      {
                          SD_Type = SD_TYPE_V2HC;    //检查CCS 大容量卡
                      }
                      else 
                          SD_Type = SD_TYPE_V2;    
                  }
              }
             
         }
    }
    else //正在初始化中非IDLE
    {
    
    }

    SD_DisSel();
    SPI1_ReadWriteByte(0xff);
    SPI1_SetSpeed(SPI_SPEED_2);
    return err;
}

/*****************************************************************
*
*
*****************************************************************/
uint8_t GetResponse(uint8_t response)
{
    u16 count=0xffff;
    uint8_t res =0;
    int err;

    do
    {
        res = SPI1_ReadWriteByte(0xff);
        //printf("waite return :%d\n",res);
      
    }while(res != response && count--);
    
    if(count==0)
    {
        err = MSD_RESPONSE_FAILURE;
    }
    else err = MSD_RESPONSE_NO_ERROR;

    return err;
}
 

/*****************************************************************
*
*
*****************************************************************/
uint8_t SD_RevData(uint8_t *buf,u16 len)
{
    if(GetResponse(0XFE)) //起始令牌
    {
        return 0xff;
    }
    while(len--)
		{
        *buf=SPI1_ReadWriteByte(0xff);
        buf++;
    }
    SPI1_ReadWriteByte(0xff);
    SPI1_ReadWriteByte(0xff);
    return 0;
}
 

/*****************************************************************
*读取扇区（缓存，扇区地址，扇区数）
*CMD17 参数指定 地址，响应R1
*****************************************************************/
uint8_t SD_ReadDisk(uint8_t *buf,u32 sec,uint8_t cnt)
{
    uint8_t err;
    if(SD_Type != SD_TYPE_V2HC) 
    {
        sec <<= 9;
    }
    if(cnt==1)
    {
        err = SendCmd(CMD17,sec,0x01);  //读取单块，标准容量卡大小由特定指令指定，高容量卡固定为512byte
        if(err == 0) //r1
        {
            err = SD_RevData(buf,512);

        }       

    }else
    {
        err = SendCmd(CMD18,sec,0x01);
        do
        {
            err = SD_RevData(buf,512);
            buf += 512;
        }while(--cnt && (err == 0));  //读取多块
        err = SendCmd(CMD12,0,0x1);   //停止指令
        //err = 0;
    }
    SD_DisSel();
    return err;

}
 



/***************************************************************************
*写入一个数据包的内容 512 字节
*cmd :指令
*响应 t : bit3 -bit1
*         010 接收数据
*         101 crc错误
*         110 数据写错误
***************************************************************************/
uint8_t SD_sendBlock(uint8_t *buf,uint8_t cmd)
{
    u16 t;
    uint8_t res = 0;
    if(SD_Waite_Ready())
    {
       return 0x1;//写失败
    }
    
    SPI1_ReadWriteByte(cmd);
    if(cmd != 0xFD)//不是指令
    {   
        for(t=0;t<512;t++)
        {
            res = SPI1_ReadWriteByte(buf[t]);
            #if 0
            if((res&0x1F) != 0x5)
            {
                printf("写入错误\n");
            }
            #endif
        }
        res = SPI1_ReadWriteByte(0xff);
        res = SPI1_ReadWriteByte(0xff);
        t = SPI1_ReadWriteByte(0xff); //t&0xe 010 正确 101 crc错误 110 写错误
        

        if((t&0x1f) != 0x05)
        {
            return 2;     //数据写入错误
        }

    }
    return 0;		
}
 
/***************************************************************************
*
*
***************************************************************************/ 
uint8_t SD_WriteDisk(uint8_t*buf,u32 sector,uint8_t cnt)
{
    uint8_t err;
    if(SD_Type != SD_TYPE_V2HC)
        sector <<= 9;//???????
    if(cnt == 1)
    {
        err = SendCmd(CMD24,sector,0x01);//sector 块地址
        if(err == 0)//??????
        {
            err = SD_sendBlock(buf,0xFE);//?512???	   
        }
    }
    else
    {
        if(SD_Type != SD_TYPE_MMC)
        {
            SendCmd(CMD55,0,0X01);	
            SendCmd(CMD23,cnt,0X01);
        }
        
        err = SendCmd(CMD25,sector,0X01);
        if(err == 0)
        {
            do
            {
                err=SD_sendBlock(buf,0xFC); 
                buf += 512;  
            }while(--cnt && err == 0);
            err=SD_sendBlock(0,0xFD);  //传输结束符号，与起始令牌相对应
        }
    }   
    SD_DisSel();
    return err;
}


 /***************************************************************************
*获取容量 速度信息
*
***************************************************************************/
uint8_t SD_GetCSD(uint8_t *buf)
{
    uint8_t r;
    r=SendCmd(CMD9,0,0X01);
    if(r == 0)
    {
          r = SD_RevData(buf,16);

    }
    SD_DisSel();
    if(r)
    {
        return 0xff;
    }
			else return 0;

}
 
 
 
/***************************************************************************
*获取制造商信息
*
***************************************************************************/
uint8_t SD_GetCID(uint8_t *cid_data)
{
    uint8_t err;	   
  
    err=SendCmd(CMD10,0,0x01);
    if(err == 0x00)
    {
        err = SD_RevData(cid_data,16);
    }
    
    SD_DisSel();
    if(err)
    {
        return 0xff;
    }
    else return 0;
}
 
/***************************************************************************
*获取扇区数
*
***************************************************************************/
u32 SD_GetSectorCount(void)
{   
    uint8_t n;
    uint8_t csd[16];
    u16 csize;  					    
    u32 Capacity; 
  
    if(SD_GetCSD(csd) != 0) 
    {
        return 0;
    }	    

    if((csd[0]&0xC0)==0x40)	 //V2.00??
    {	
        csize = csd[9] + ((u16)csd[8] << 8) + 1;
        Capacity = (u32)csize << 10; 		   
    }
    else//V1.XX??
    {	
        n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
        csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
        Capacity= (u32)csize << (n - 9);//?????   
    }
    return Capacity;
} 
 

 
 
 
 
 
 
