#ifndef __SDX1_H__
#define __SDX1_H__


#include "sd_type.h"

#include "stm32f10x.h"




#define SET_SD1_CS_PIN(val)  (val?GPIO_SetBits(GPIOD, GPIO_Pin_11):GPIO_ResetBits(GPIOD, GPIO_Pin_11))

#define TRY_TIME 200   //��SD��д������֮�󣬶�ȡSD���Ļ�Ӧ����������TRY_TIME�Σ������TRY_TIME���ж�������Ӧ��������ʱ��������д��ʧ��

//��غ궨��
//-------------------------------------------------------------
#define SD_VER_ERR     0X00
#define SD_VER_MMC     0X01
#define SD_VER_V1      0X02
#define SD_VER_V2      0X03
#define SD_VER_V2HC    0X04

#define INIT_ERROR                  0x01 //��ʼ������
#define INIT_CMD0_ERROR             0x02 //CMD0����
#define INIT_CMD1_ERROR             0x03 //CMD1����
#define INIT_SDV2_ACMD41_ERROR	    0x04 //ACMD41����
#define INIT_SDV1_ACMD41_ERROR	    0x05 //ACMD41����

#define WRITE_CMD24_ERROR           0x06 //д��ʱ����CMD24����
#define WRITE_BLOCK_ERROR           0x07 //д�����

#define READ_BLOCK_ERROR            0x08 //�������

#define WRITE_CMD25_ERROR           0x09 //���������дʱ����CMD25����
#define WRITE_NBLOCK_ERROR          0x0A //�������дʧ��

#define READ_CMD18_ERROR            0x0B //����������ʱ����CMD18����
 
#define GET_CSD_ERROR               0x0C //��CSDʧ��

//-------------------------------------------------------------
UINT8 SD1_Init(void); //SD����ʼ��

UINT8 SD1_Write_Sector(UINT32 addr,UINT8 *buffer); //��buffer���ݻ������е�����д���ַΪaddr��������
UINT8 SD1_Read_Sector(UINT32 addr,UINT8 *buffer);	 //�ӵ�ַΪaddr�������ж�ȡ���ݵ�buffer���ݻ�������
UINT8 SD1_Write_nSector(UINT32 nsec,UINT32 addr,UINT8 *buffer); //��buffer���ݻ������е�����д����ʼ��ַΪaddr��nsec������������
UINT8 SD1_Read_nSector(UINT32 nsec,UINT32 addr,UINT8 *buffer); //��buffer���ݻ������е�����д����ʼ��ַΪaddr��nsec������������
UINT8 SD1_Erase_nSector(UINT32 addr_sta,UINT32 addr_end);
UINT32 SD1_GetTotalSec(void); //��ȡSD������������


u8 SPI_SD_SendReadByte(u8 TxData);
void SD_Flash_Init(u8 SPI_BaudRatePrescaler);

void SD1_SPI_SPEED_LOW(void);
void SD1_SPI_SPEED_HIGH(void);

#endif