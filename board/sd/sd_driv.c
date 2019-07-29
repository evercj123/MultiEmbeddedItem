/*****************************************************************
*���ݴ���ʱbitλ˳��
*SDģʽ��
*Usual data (8-bit width): The usual data (8-bit width) are sent in LSB (Least Significant Byte) first,
*MSB (Most Significant Byte) last sequence. But in the individual byte, it is MSB (Most Significant Bit)
*first, LSB (Least Significant Bit) last.
*Wide width data (SD Memory Register): The wide width data is shifted from the MSB bit
*
*SPIģʽ��
*****************************************************************/



#include "stm32f10x.h"
#include "sd_driv.h"
#include "spi.h"
#include "Head.h"
uint8_t  SD_Type;			//SD��������
uint8_t  UERR;
void pin_init(void)
{

			RCC->APB2ENR|=1<<5;			//PD				
			GPIOD->CRL&=0XFFFFF0FF;
			GPIOD->CRL|=0X00000300;
			GPIOD->ODR|=0X1<<2; 
}
 

/*****************************************************************
*���� 0: OK    0xff:error
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
 * SD��ʧ��
 *
 *****************************************************************/
 void SD_DisSel(void)
 {
    SD_CS=1;
    UERR = SPI1_ReadWriteByte(0xff);//����Ӧ��8��ʱ��
 }
 
 
 /*****************************************************************
 *Ƭѡ ��ʹ��
 *д��ָ�����8��ʱ��
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
        SPI1_ReadWriteByte(0XFF);		//ֹͣ���ݴ���
    }
    retry=0x1f;	
    do
    {
        err = SPI1_ReadWriteByte(0xff); //�ȴ���ʼ��Ӧ 
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
    return 0;  //���سɹ�
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
        SPI1_ReadWriteByte(0xff);  //��������74��ʱ��
    }
    retry = 20;
    do
    {
        err = SendCmd(CMD0,0,0x95);   //�������
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
          if(buff[2] == 0X01 && buff[3] == 0XAA)//���Ƿ�֧��2.7~3.6V
          {
              retry = 0XFFFE;
              do
              {
                  SendCmd(CMD55,0,0X01);	//����CMD55				
                  err = SendCmd(CMD41,0x40000000,0X01);//�Ƿ��Ǹ������� 0x40000000 HCS
              }while(err && retry--);

              if(retry && SendCmd(CMD58,0,0X01) == 0)//����SD2.0���汾��ʼ(R3register)
              {
                  for(i = 0;i < 4;i++)
                  {     
                       buff[i] = SPI1_ReadWriteByte(0XFF);//�õ�OCRֵ
                  }
                  if(buff[0] & 0x40)  //bit30 card capacity status
                  {
                       SD_Type = SD_TYPE_V2HC;    //���CCS ��������
                  }
                  else 
                       SD_Type = SD_TYPE_V2;  
              }

          }

      }

    }
    else
    {
        SendCmd(CMD55,0,0X01);		//ָ����������
        err = SendCmd(CMD41,0,0X01);	//��ȡ�������� orc�Ĵ���
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
    }while((err != 0xff ) && cnt--); //����
    if(err == 0x01)//IDLE״̬��OR��ʼ��״̬��
    {
         err = SendCmd(CMD8,0x1AA,0x87);
         if(err == 0X01)
         {
              uint8_t buf[4];
              for(uint8_t i = 0;i < 4;i++)
              {
                  buf[i] = SPI1_ReadWriteByte(0xff);
              }
//            if(buf[2]) //����ѹ

              int retry = 200;
              do
              {   SendCmd(CMD55,0,0x01);
                  err = SendCmd(CMD41,0x40000000,0X01);//r1���� 1 :���ڳ�ʼ����
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
                          SD_Type = SD_TYPE_V2HC;    //���CCS ��������
                      }
                      else 
                          SD_Type = SD_TYPE_V2;    
                  }
              }
             
         }
    }
    else //���ڳ�ʼ���з�IDLE
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
    if(GetResponse(0XFE)) //��ʼ����
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
*��ȡ���������棬������ַ����������
*CMD17 ����ָ�� ��ַ����ӦR1
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
        err = SendCmd(CMD17,sec,0x01);  //��ȡ���飬��׼��������С���ض�ָ��ָ�������������̶�Ϊ512byte
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
        }while(--cnt && (err == 0));  //��ȡ���
        err = SendCmd(CMD12,0,0x1);   //ָֹͣ��
        //err = 0;
    }
    SD_DisSel();
    return err;

}
 



/***************************************************************************
*д��һ�����ݰ������� 512 �ֽ�
*cmd :ָ��
*��Ӧ t : bit3 -bit1
*         010 ��������
*         101 crc����
*         110 ����д����
***************************************************************************/
uint8_t SD_sendBlock(uint8_t *buf,uint8_t cmd)
{
    u16 t;
    uint8_t res = 0;
    if(SD_Waite_Ready())
    {
       return 0x1;//дʧ��
    }
    
    SPI1_ReadWriteByte(cmd);
    if(cmd != 0xFD)//����ָ��
    {   
        for(t=0;t<512;t++)
        {
            res = SPI1_ReadWriteByte(buf[t]);
            #if 0
            if((res&0x1F) != 0x5)
            {
                printf("д�����\n");
            }
            #endif
        }
        res = SPI1_ReadWriteByte(0xff);
        res = SPI1_ReadWriteByte(0xff);
        t = SPI1_ReadWriteByte(0xff); //t&0xe 010 ��ȷ 101 crc���� 110 д����
        

        if((t&0x1f) != 0x05)
        {
            return 2;     //����д�����
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
        err = SendCmd(CMD24,sector,0x01);//sector ���ַ
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
            err=SD_sendBlock(0,0xFD);  //����������ţ�����ʼ�������Ӧ
        }
    }   
    SD_DisSel();
    return err;
}


 /***************************************************************************
*��ȡ���� �ٶ���Ϣ
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
*��ȡ��������Ϣ
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
*��ȡ������
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
 

 
 
 
 
 
 
