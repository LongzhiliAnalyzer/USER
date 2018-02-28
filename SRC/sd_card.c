#include "sd_card.h"
#include "myfun.h"
#include "sd_type.h"

//��������
//--------------------------------------------------------------

UINT8 SD1_Addr_Mode=0; //SD1��Ѱַ��ʽ��1Ϊ��Ѱַ��0Ϊ�ֽ�Ѱַ
UINT8 SD1_Ver=SD_VER_ERR; //SD��1�İ汾
//---------------------------------------------------------------

#define SD1_SPI_WByte(x) SPI_SD_SendReadByte(x)

#define SD1_SPI_RByte()  SPI_SD_SendReadByte(0XFF)

void SD_Flash_Init(u8 SPI_BaudRatePrescaler)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOA|RCC_APB2Periph_SPI1 ,ENABLE);
	
	//SPI1�ܽų�ʼ����SPI����
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//MISO�ܽ�
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	//FLASH_CS�ܽ�
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	SET_SD1_CS_PIN(1);
	
	//SPI1��ʼ��
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
	
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;/*SPI_NSS_Hard;*/
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStruct);

	/* Enable SPI1  */
	SPI_Cmd(SPI1, ENABLE);

}

u8 SPI_SD_SendReadByte(u8 TxData)
{

	while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET);
	
	SPI_I2S_SendData(SPI1, TxData);

    /* Wait to receive a Half Word */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return the Half Word read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);
}

void SD1_SPI_SPEED_LOW(void) 
{
	SD_Flash_Init(SPI_BaudRatePrescaler_128); 
}

void SD1_SPI_SPEED_HIGH(void)
{
	SD_Flash_Init(SPI_BaudRatePrescaler_4); 
}

UINT8 SD1_SPI_Init(void)
{
 SD_Flash_Init(SPI_BaudRatePrescaler_128); //SPI�ӿڳ�ʼ��
	
 return 0;
}

/******************************************************************
 - ������������SD��д����
 - ����˵����SD����������6���ֽڣ�pcmd��ָ�������ֽ����е�ָ��
 - ����˵��������д�벻�ɹ���������0xff
 ******************************************************************/

UINT8 SD1_Write_Cmd(UINT8 *pcmd) 
{
 UINT8 r=0,time=0;
 
 SET_SD1_CS_PIN(1);
 SD1_SPI_WByte(0xFF); //����8��ʱ�ӣ���߼����ԣ����û�������ЩSD�����ܲ�֧��   
	
 SET_SD1_CS_PIN(0);
 while(0XFF!=SD1_SPI_RByte()); //�ȴ�SD��׼���ã������䷢������

 //��6�ֽڵ���������д��SD��
 SD1_SPI_WByte(pcmd[0]);
 SD1_SPI_WByte(pcmd[1]);
 SD1_SPI_WByte(pcmd[2]);
 SD1_SPI_WByte(pcmd[3]);
 SD1_SPI_WByte(pcmd[4]);
 SD1_SPI_WByte(pcmd[5]);
	
 if(pcmd[0]==0X1C) SD1_SPI_RByte(); //�����ֹͣ�������������ֽ�

 do 
 {  
  r=SD1_SPI_RByte();
  time++;
 }while((r&0X80)&&(time<TRY_TIME)); //������Դ�������TRY_TIME�򷵻ش���

 return r;
}

/******************************************************************
 - ����������SD����ʼ��������ڲ�ͬ��SD������MMC��SD��SDHC����ʼ��
             �����ǲ�ͬ��
 - ����˵������
 - ����˵�������óɹ�������0x00�����򷵻ش�����
 ******************************************************************/

UINT8 SD1_Init(void)
{
 UINT8 time=0,r=0,i=0;
	
 UINT8 rbuf[4]={0};
	
 UINT8 pCMD0[6] ={0x40,0x00,0x00,0x00,0x00,0x95}; //CMD0����SD����Ĭ���ϵ���SDģʽ�л���SPIģʽ��ʹSD������IDLE״̬
 UINT8 pCMD1[6] ={0x41,0x00,0x00,0x00,0x00,0x01}; //CMD1��MMC��ʹ��CMD1������г�ʼ��
 UINT8 pCMD8[6] ={0x48,0x00,0x00,0x01,0xAA,0x87}; //CMD8�����ڼ���SD���İ汾�����ɴ�Ӧ���֪SD���Ĺ�����ѹ
 UINT8 pCMD16[6]={0x50,0x00,0x00,0x02,0x00,0x01}; //CMD16������������СΪ512�ֽڣ������������ڳ�ʼ�����֮�������̽�ԵĲ�����
                                                          //��������ɹ���˵����ʼ��ȷʵ�ɹ�
 UINT8 pCMD55[6]={0x77,0x00,0x00,0x00,0x00,0x01}; //CMD55�����ڸ�֪SD��������ACMD����Ӧ�ò����� CMD55+ACMD41���ʹ��
                                                          //MMC��ʹ��CMD1�����г�ʼ������SD����ʹ��CMD55+ACMD41�����г�ʼ��
 UINT8 pACMD41H[6]={0x69,0x40,0x00,0x00,0x00,0x01}; //ACMD41,���������ڼ��SD���Ƿ��ʼ����ɣ�MMC���������ô�������2.0��SD��
 UINT8 pACMD41S[6]={0x69,0x00,0x00,0x00,0x00,0x01}; //ACMD41,���������ڼ��SD���Ƿ��ʼ����ɣ�MMC���������ô�������1.0��SD��

 UINT8 pCMD58[6]={0x7A,0x00,0x00,0x00,0x00,0x01}; //CMD58�����ڼ���SD2.0������SDHC��������ͨ��SD�������߶�������ַ��Ѱַ��ʽ��ͬ
 
 SD1_SPI_Init(); //SPI�ӿ���س�ʼ��

 SD1_SPI_SPEED_LOW(); //���Ƚ�SPI��Ϊ����
	
 SET_SD1_CS_PIN(1); 
	
 for(i=0;i<0x0f;i++) //����Ҫ��������74��ʱ���źţ����Ǳ���ģ�����SD��
 {
  SD1_SPI_WByte(0xff); //120��ʱ��
 }

 time=0;
 do
 { 
  r=SD1_Write_Cmd(pCMD0);//д��CMD0
  time++;
  if(time>=TRY_TIME) 
  { 
   return(INIT_CMD0_ERROR);//CMD0д��ʧ��
  }
 }while(r!=0x01);
 
 if(1==SD1_Write_Cmd(pCMD8))//д��CMD8���������ֵΪ1����SD���汾Ϊ2.0
 {
	rbuf[0]=SD1_SPI_RByte(); rbuf[1]=SD1_SPI_RByte(); //��ȡ4���ֽڵ�R7��Ӧ��ͨ������֪��SD���Ƿ�֧��2.7~3.6V�Ĺ�����ѹ
	rbuf[2]=SD1_SPI_RByte(); rbuf[3]=SD1_SPI_RByte();
	 
	if(rbuf[2]==0X01 && rbuf[3]==0XAA)//SD���Ƿ�֧��2.7~3.6V
	{		
	 time=0;
	 do
	 {
		SD1_Write_Cmd(pCMD55);//д��CMD55
		r=SD1_Write_Cmd(pACMD41H);//д��ACMD41�����SD2.0
		time++;
    if(time>=TRY_TIME) 
    { 
     return(INIT_SDV2_ACMD41_ERROR);//��SD2.0ʹ��ACMD41���г�ʼ��ʱ��������
    }
   }while(r!=0);	

   if(0==SD1_Write_Cmd(pCMD58)) //д��CMD58����ʼ����SD2.0
   {
	  rbuf[0]=SD1_SPI_RByte(); rbuf[1]=SD1_SPI_RByte(); //��ȡ4���ֽڵ�OCR������CCSָ������SDHC������ͨ��SD
	  rbuf[2]=SD1_SPI_RByte(); rbuf[3]=SD1_SPI_RByte();	

    if(rbuf[0]&0x40) 
		{
		 SD1_Ver=SD_VER_V2HC; //SDHC��	
		 SD1_Addr_Mode=1; //SDHC��������Ѱַ��ʽ��������ַ
		}	
    else SD1_Ver=SD_VER_V2; //��ͨ��SD����2.0�Ŀ�����SDHC��һЩ��ͨ�Ŀ�				
   }
  }
 }
 else //SD V1.0��MMC 
 {
	//SD��ʹ��ACMD41���г�ʼ������MMCʹ��CMD1�����г�ʼ������������һ���ж���SD����MMC
	SD1_Write_Cmd(pCMD55);//д��CMD55
	r=SD1_Write_Cmd(pACMD41S);//д��ACMD41�����SD1.0
    
  if(r<=1) //��鷵��ֵ�Ƿ���ȷ�������ȷ��˵��ACMD41������ܣ���ΪSD��
  {
	 SD1_Ver=SD_VER_V1; //��ͨ��SD1.0����һ����˵�������ᳬ��2G
			
	 time=0;
	 do
	 {
		SD1_Write_Cmd(pCMD55);//д��CMD55
		r=SD1_Write_Cmd(pACMD41S);//д��ACMD41�����SD1.0
		time++;
    if(time>=TRY_TIME) 
    { 
     return(INIT_SDV1_ACMD41_ERROR);//��SD1.0ʹ��ACMD41���г�ʼ��ʱ��������
    }
   }while(r!=0);			 
  }
  else //����ΪMMC	
	{
	 SD1_Ver=SD_VER_MMC; //MMC��������֧��ACMD41�������ʹ��CMD1���г�ʼ��
			
	 time=0;
   do
   { 
    r=SD1_Write_Cmd(pCMD1);//д��CMD1
    time++;
    if(time>=TRY_TIME) 
    { 
     return(INIT_CMD1_ERROR);//MMC��ʹ��CMD1������г�ʼ���в�������
    }
   }while(r!=0);			
  }
 }
 
 if(0!=SD1_Write_Cmd(pCMD16)) //SD���Ŀ��С����Ϊ512�ֽ�
 {
	SD1_Ver=SD_VER_ERR; //������ɹ�����˿�Ϊ�޷�ʶ��Ŀ�
	return INIT_ERROR;
 }	
 
 SET_SD1_CS_PIN(1);
 SD1_SPI_WByte(0xFF); //����SD���Ĳ���ʱ�������ﲹ8��ʱ�� 
 
 SD1_SPI_SPEED_HIGH(); //SPI�е�����
 
 return 0;//����0,˵����λ�����ɹ�
}

/******************************************************************
 - ������������SD�����ɸ��������в����������������е����ݴ󲿷����
             ��Ϊȫ0����Щ��������Ϊȫ0XFF����Ҫʹ�ô˺�������ȷ�ϣ�
 - ����˵����addr_sta����ʼ������ַ   addr_end������������ַ
 - ����˵�������óɹ�������0x00�����򷵻ش�����
 ******************************************************************/

UINT8 SD1_Erase_nSector(UINT32 addr_sta,UINT32 addr_end)
{
 UINT8 r,time;
// UINT8 i=0;
 UINT8 pCMD32[]={0x60,0x00,0x00,0x00,0x00,0xff}; //���ò����Ŀ�ʼ������ַ
 UINT8 pCMD33[]={0x61,0x00,0x00,0x00,0x00,0xff}; //���ò����Ľ���������ַ
 UINT8 pCMD38[]={0x66,0x00,0x00,0x00,0x00,0xff}; //��������

 if(!SD1_Addr_Mode) {addr_sta<<=9;addr_end<<=9;} //addr = addr * 512	�����ַ��������ַ��תΪ�ֽڵ�ַ

 pCMD32[1]=addr_sta>>24; //����ʼ��ַд�뵽CMD32�ֽ�������
 pCMD32[2]=addr_sta>>16;
 pCMD32[3]=addr_sta>>8;
 pCMD32[4]=addr_sta;	 

 pCMD33[1]=addr_end>>24; //����ʼ��ַд�뵽CMD32�ֽ�������
 pCMD33[2]=addr_end>>16;
 pCMD33[3]=addr_end>>8;
 pCMD33[4]=addr_end;	

 time=0;
 do
 {  
  r=SD1_Write_Cmd(pCMD32);
  time++;
  if(time==TRY_TIME) 
  { 
   return(r); //����д��ʧ��
  }
 }while(r!=0);  
 
 time=0;
 do
 {  
  r=SD1_Write_Cmd(pCMD33);
  time++;
  if(time==TRY_TIME) 
  { 
   return(r); //����д��ʧ��
  }
 }while(r!=0);  
 
 time=0;
 do
 {  
  r=SD1_Write_Cmd(pCMD38);
  time++;
  if(time==TRY_TIME) 
  { 
   return(r); //����д��ʧ��
  }
 }while(r!=0);

 return 0; 

}

/****************************************************************************
 - ������������bufferָ���512���ֽڵ�����д�뵽SD����addr������
 - ����˵����addr:������ַ
             buffer:ָ�����ݻ�������ָ��
 - ����˵�������óɹ�������0x00�����򷵻ش�����
 - ע��SD����ʼ���ɹ��󣬶�д����ʱ��������SPI�ٶ������������Ч��
 ****************************************************************************/

UINT8 SD1_Write_Sector(UINT32 addr,UINT8 *buffer)	//��SD���е�ָ����ַ������д��512���ֽڣ�ʹ��CMD24��24�����
{  
 UINT8 r,time;
 UINT8 i=0;
 UINT8 pCMD24[]={0x58,0x00,0x00,0x00,0x00,0xff}; //��SD���е����飨512�ֽڣ�һ��������д�����ݣ���CMD24

 if(!SD1_Addr_Mode) addr<<=9; //addr = addr * 512	�����ַ��������ַ��תΪ�ֽڵ�ַ

 pCMD24[1]=addr>>24; //���ֽڵ�ַд�뵽CMD24�ֽ�������
 pCMD24[2]=addr>>16;
 pCMD24[3]=addr>>8;
 pCMD24[4]=addr;	

 time=0;
 do
 {  
  r=SD1_Write_Cmd(pCMD24);
  time++;
  if(time==TRY_TIME) 
  { 
   return(r); //����д��ʧ��
  }
 }while(r!=0); 

 while(0XFF!=SD1_SPI_RByte()); //�ȴ�SD��׼���ã������䷢���������������
	
 SD1_SPI_WByte(0xFE);//д�뿪ʼ�ֽ� 0xfe���������Ҫд���512���ֽڵ�����	
	
 for(i=0;i<4;i++) //����������Ҫд���512���ֽ�д��SD1��������ѭ���������������д���ٶ�
 {
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
 }
  
 SD1_SPI_WByte(0xFF); 
 SD1_SPI_WByte(0xFF); //�����ֽڵ�CRCУ���룬���ù���
       
 r=SD1_SPI_RByte();   //��ȡ����ֵ
 if((r & 0x1F)!=0x05) //�������ֵ�� XXX00101 ˵�������Ѿ���SD��������
 {
  return(WRITE_BLOCK_ERROR); //д������ʧ��
 }
 
 while(0xFF!=SD1_SPI_RByte());//�ȵ�SD����æ�����ݱ������Ժ�SD��Ҫ����Щ����д�뵽�����FLASH�У���Ҫһ��ʱ�䣩
						                 //æʱ����������ֵΪ0x00,��æʱ��Ϊ0xff

 SET_SD1_CS_PIN(1);
 SD1_SPI_WByte(0xFF); //����SD���Ĳ���ʱ�������ﲹ8��ʱ�� 
 
 return(0);		 //����0,˵��д���������ɹ�
} 

/****************************************************************************
 - ������������ȡaddr������512���ֽڵ�bufferָ������ݻ�����
 - ����˵����addr:������ַ
             buffer:ָ�����ݻ�������ָ��
 - ����˵�������óɹ�������0x00�����򷵻ش�����
 - ע��SD����ʼ���ɹ��󣬶�д����ʱ��������SPI�ٶ������������Ч��
 ****************************************************************************/

UINT8 SD1_Read_Sector(UINT32 addr,UINT8 *buffer)//��SD����ָ�������ж���512���ֽڣ�ʹ��CMD17��17�����
{
 UINT8 i;
 UINT8 time,r;
	
 UINT8 pCMD17[]={0x51,0x00,0x00,0x00,0x00,0x01}; //CMD17���ֽ�����
   
 if(!SD1_Addr_Mode) addr<<=9; //sector = sector * 512	   �����ַ��������ַ��תΪ�ֽڵ�ַ

 pCMD17[1]=addr>>24; //���ֽڵ�ַд�뵽CMD17�ֽ�������
 pCMD17[2]=addr>>16;
 pCMD17[3]=addr>>8;
 pCMD17[4]=addr;	

 time=0;
 do
 {  
  r=SD1_Write_Cmd(pCMD17); //д��CMD17
  time++;
  if(time==TRY_TIME) 
  {
   return(READ_BLOCK_ERROR); //����ʧ��
  }
 }while(r!=0); 
   			
 while(SD1_SPI_RByte()!= 0xFE); //һֱ����������0xfeʱ��˵���������512�ֽڵ�������

 for(i=0;i<4;i++)	 //������д�뵽���ݻ�������
 {	
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
 }

 SD1_SPI_RByte();
 SD1_SPI_RByte();//��ȡ�����ֽڵ�CRCУ���룬���ù�������

 SET_SD1_CS_PIN(1);
 SD1_SPI_WByte(0xFF); //����SD1���Ĳ���ʱ�������ﲹ8��ʱ�� 

 return 0;
}

/****************************************************************************
 - ������������addr������ʼ��nsec������д�����ݣ���Ӳ��������д�룩
 - ����˵����nsec:������
             addr:��ʼ������ַ
             buffer:ָ�����ݻ�������ָ��
 - ����˵�������óɹ�������0x00�����򷵻ش�����
 - ע��SD����ʼ���ɹ��󣬶�д����ʱ��������SPI�ٶ������������Ч��
 ****************************************************************************/

UINT8 SD1_Write_nSector(UINT32 nsec,UINT32 addr,UINT8 *buffer)	
{  
 UINT8 r,time;
 UINT32 i=0,j=0;
	
 UINT8 pCMD25[6]={0x59,0x00,0x00,0x00,0x00,0x01}; //CMD25������ɶ������д
 UINT8 pCMD55[6]={0x77,0x00,0x00,0x00,0x00,0x01}; //CMD55�����ڸ�֪SD��������ACMD,CMD55+ACMD23
 UINT8 pACMD23[6]={0x57,0x00,0x00,0x00,0x00,0x01};//CMD23���������Ԥ����

 if(!SD1_Addr_Mode) addr<<=9; 

 pCMD25[1]=addr>>24;
 pCMD25[2]=addr>>16;
 pCMD25[3]=addr>>8;
 pCMD25[4]=addr;

 pACMD23[1]=nsec>>24;
 pACMD23[2]=nsec>>16;
 pACMD23[3]=nsec>>8;
 pACMD23[4]=nsec; 

 if(SD1_Ver!=SD_VER_MMC) //�������MMC����������д��Ԥ�������CMD55+ACMD23�����������������д���ٶȻ����
 {
	SD1_Write_Cmd(pCMD55);
	SD1_Write_Cmd(pACMD23);
 }

 time=0;
 do
 {  
  r=SD1_Write_Cmd(pCMD25);
  time++;
  if(time==TRY_TIME) 
  { 
   return(WRITE_CMD25_ERROR); //����д��ʧ��
  }
 }while(r!=0); 

 while(0XFF!=SD1_SPI_RByte()); //�ȴ�SD��׼���ã������䷢���������������

 for(j=0;j<nsec;j++)
 {
  SD1_SPI_WByte(0xFC);//д�뿪ʼ�ֽ� 0xfc���������Ҫд���512���ֽڵ�����	
	
  for(i=0;i<4;i++) //����������Ҫд���512���ֽ�д��SD��
  {
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
   SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));SD1_SPI_WByte(*(buffer++));
  }
  
  SD1_SPI_WByte(0xFF); 
  SD1_SPI_WByte(0xFF); //�����ֽڵ�CRCУ���룬���ù���
       
  r=SD1_SPI_RByte();   //��ȡ����ֵ
  if((r & 0x1F)!=0x05) //�������ֵ�� XXX00DELAY_TIME1 ˵�������Ѿ���SD��������
  {
   return(WRITE_NBLOCK_ERROR); //д������ʧ��
  }
 
  while(0xFF!=SD1_SPI_RByte());//�ȵ�SD����æ�����ݱ������Ժ�SD��Ҫ����Щ����д�뵽�����FLASH�У���Ҫһ��ʱ�䣩
						                   //æʱ����������ֵΪ0x00,��æʱ��Ϊ0xff
 }

 SD1_SPI_WByte(0xFD);

 while(0xFF!=SD1_SPI_RByte());

 SET_SD1_CS_PIN(1);//�ر�Ƭѡ

 SD1_SPI_WByte(0xFF);//����SD���Ĳ���ʱ�������ﲹ8��ʱ��

 return(0);		 //����0,˵��д���������ɹ�
} 

/****************************************************************************
 - ������������ȡaddr������ʼ��nsec�����������ݣ���Ӳ����������ȡ��
 - ����˵����nsec:������
             addr:��ʼ������ַ
             buffer:ָ�����ݻ�������ָ��
 - ����˵�������óɹ�������0x00�����򷵻ش�����
 - ע��SD����ʼ���ɹ��󣬶�д����ʱ��������SPI�ٶ������������Ч��
 ****************************************************************************/

UINT8 SD_Read_nSector(UINT32 nsec,UINT32 addr,UINT8 *buffer)
{
 UINT8 r,time;
 UINT32 i=0,j=0;
	
 UINT8 pCMD18[6]={0x52,0x00,0x00,0x00,0x00,0x01}; //CMD18���ֽ�����
 UINT8 pCMD12[6]={0x1C,0x00,0x00,0x00,0x00,0x01}; //CMD12��ǿ��ֹͣ����
   
 if(!SD1_Addr_Mode) addr<<=9; //sector = sector * 512	   �����ַ��������ַ��תΪ�ֽڵ�ַ,���SD1_Addr_Mode=0��Ϊ�ֽ�Ѱַ

 pCMD18[1]=addr>>24; //���ֽڵ�ַд�뵽CMD17�ֽ�������
 pCMD18[2]=addr>>16;
 pCMD18[3]=addr>>8;
 pCMD18[4]=addr;	

 time=0;
 do
 {  
  r=SD1_Write_Cmd(pCMD18); //д��CMD18
  time++;
  if(time==TRY_TIME) 
  {
   return(READ_CMD18_ERROR); //д��CMD18ʧ��
  }
 }while(r!=0); 
 
 for(j=0;j<nsec;j++)
 {  
  while(SD1_SPI_RByte()!= 0xFE); //һֱ����������0xfeʱ��˵���������512�ֽڵ�������
 
  for(i=0;i<4;i++)	 //������д�뵽���ݻ�������
  {	
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
   *(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();*(buffer++)=SD1_SPI_RByte();
  }
 
  SD1_SPI_RByte();
  SD1_SPI_RByte();//��ȡ�����ֽڵ�CRCУ���룬���ù�������
 }

 SD1_Write_Cmd(pCMD12); //д��CMD12���ֹͣ���ݶ�ȡ 

 SET_SD1_CS_PIN(1);
 SD1_SPI_WByte(0xFF); //����SD���Ĳ���ʱ�������ﲹ8��ʱ�� 

 return 0;
}

/****************************************************************************
 - ������������ȡSD��������������ͨ����ȡSD����CSD���������õ�����������
 - ����˵������
 - ����˵��������SD������������
 - ע����
 ****************************************************************************/

UINT32 SD1_GetTotalSec(void)
{
 UINT8 pCSD[16];
 UINT32 Capacity;  
 UINT8 n,i;
 UINT16 csize; 

 UINT8 pCMD9[6]={0x49,0x00,0x00,0x00,0x00,0x01}; //CMD9	

 if(SD1_Write_Cmd(pCMD9)!=0) //д��CMD9����
 {
	return GET_CSD_ERROR; //��ȡCSDʱ��������
 }

 while(SD1_SPI_RByte()!= 0xFE); //һֱ����������0xfeʱ��˵���������16�ֽڵ�CSD����

 for(i=0;i<16;i++) pCSD[i]=SD1_SPI_RByte(); //��ȡCSD����

 SD1_SPI_RByte();
 SD1_SPI_RByte(); //��ȡ�����ֽڵ�CRCУ���룬���ù�������

 SET_SD1_CS_PIN(1);
 SD1_SPI_WByte(0xFF); //����SD���Ĳ���ʱ�������ﲹ8��ʱ�� 
	
 //���ΪSDHC�����������������������淽ʽ����
 if((pCSD[0]&0xC0)==0x40)	 //SD2.0�Ŀ�
 {	
	csize=pCSD[9]+(((UINT16)(pCSD[8]))<<8)+1;
  Capacity=((UINT32)csize)<<10;//�õ�������	 		   
 }
 else //SD1.0�Ŀ�
 {	
	n=(pCSD[5]&0x0F)+((pCSD[10]&0x80)>>7)+((pCSD[9]&0x03)<<1)+2;
	csize=(pCSD[8]>>6)+((UINT16)pCSD[7]<<2)+((UINT16)(pCSD[6]&0x03)<<0x0A)+1;
	Capacity=(UINT32)csize<<(n-9);//�õ�������   
 }
 return Capacity;
}



