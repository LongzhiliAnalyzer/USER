/*******************************************************************************
* File        : CH375.h
* Author      : 
* Date        : 
* Description : CH375 �ײ���������ͷ�ļ�
* Version     : V1.0

ע��Ӳ�����Ӳ����и��£�����

Ӳ�����ӣ�STM32F103R8T6      CH375
   	        PC0-PC7  <--->  D0-D7 ���ݶ˿�
	            PC9  <----  INT#  �ڸ�λ��ɺ�Ϊ�ж�����������͵�ƽ��Ч
                PC12  ---->  A0    ����/����ѡ��
	            PC8  ---->  CS#   Ƭѡ�ź�
		        PC10  ---->  WR#   дѡͨ
		        PC11  ---->  RD#   ��ѡͨ
*******************************************************************************/
#ifndef  _CH375_H_
#define  _CH375_H_

/* Includes ---------------------------------------------------------------*/

#include"CH375INC.h"  //CH375�������Ĵ�������
#include"stm32f10x.h" //stm32f103r8t6������ͷ�ļ�
#include "sys.h"
//#include "usart.h"

/*�˿����Ŷ���-------------------------------------------------------------*/

/* CH375 �����ݶ˿�*/
#define GPIO_CH375_Data  GPIOC
#define RCC_APB2Periph_GPIO_CH375_Data    RCC_APB2Periph_GPIOC

/* CH375 ���������ţ�˫��ģʽ�����  PC0-PC7 */
#define DATA_MODE_IN   GPIOC->CRL = 0x44444444 //GPIOC�˿ڶ���Ϊ����:IN_FLOATING 
#define DATA_MODE_OUT  GPIOC->CRL = 0x33333333 //GPIOC�˿ڶ���Ϊ���:Out_PP,50Hz 


/* CH375 �Ŀ��ƶ˿�*/ 
#define GPIO_CH375_CTL   GPIOC
#define RCC_APB2Periph_GPIO_CH375_CTL     RCC_APB2Periph_GPIOC

/* ����/����ģʽѡ�� A0: 1-д��� 0-д����*/
#define A0_Hx  GPIO_SetBits(GPIOC,GPIO_Pin_12)
#define A0_Lx GPIO_ResetBits(GPIOC,GPIO_Pin_12)

/* Ƭѡ CS#  �͵�ƽ��Ч*/ 
#define CS_Hx  GPIO_SetBits(GPIOC,GPIO_Pin_8)
#define CS_Lx  GPIO_ResetBits(GPIOC,GPIO_Pin_8)

/* дѡͨ WR# �͵�ƽ��Ч*/
#define WR_Hx  GPIO_SetBits(GPIOC,GPIO_Pin_10)
#define WR_Lx  GPIO_ResetBits(GPIOC,GPIO_Pin_10)

/* ��ѡͨ RD# �͵�ƽ��Ч*/
#define RD_Hx  GPIO_SetBits(GPIOC,GPIO_Pin_11)
#define RD_Lx  GPIO_ResetBits(GPIOC,GPIO_Pin_11)

/* Function Prototype ---------------------------------------------------*/


void CH375_DelayNus(__IO uint32_t nCount);  // CH375����ʱ��Ҫ����ʱ���������ݲ�ͬ��CPU��Ҫ������ 

void CH375_Configuration(void);        // CH375 ��Ӧʱ�ӣ����ŵ�����
 
void CH375_WriteCmd(uint8_t cmd);      // ��CH375д������
void CH375_WriteDat(uint8_t dat);      // ��CH375д������
uint8_t CH375_ReadCmd(void);           // ��CH375��ȡ����
uint8_t CH375_ReadDat(void);           // ��CH375��ȡ���ݻ�״̬
 
uint8_t CH375_WaitInterrupt(void);     // �����ȴ�CH375оƬ�ڲ�������ɲ������жϣ����ز���״̬
 
uint8_t CH375_Init(void);              // ��ʼ��CH375оƬ
uint8_t CH375_DiskConnect(void);       // �����Ƿ�����
uint8_t CH375_DiskInit(void);          // ��ʼ������
uint8_t CH375_DiskReady(void);         // �����Ƿ�׼���ã��ǹ��Ѿ�����ʼ�����õ������ַ��

uint8_t CH375_WriteSector(uint32_t addr, const uint8_t *pbuff); // ��U��һ����д������
uint8_t CH375_ReadSector(uint32_t addr, uint8_t *pbuff); // ��U��һ������������

#endif   /*  _CH375_H_  */
 

