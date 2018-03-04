/*********************************************************************
*                Copyright (C), 2015-2016, Supersonics. Co., Ltd.
*                        �迹��������������
*
*                          Ӳ��ƽ̨: xxx
*                          �� о Ƭ: STM32F103
*                          �� Ŀ ��: xxx
**********************************************************************
*�ļ���: control.h
*��  ��: V1.0.0
*��  ��: 
*��  ��: 
*˵  ��:
**********************************************************************
*��Ҫ������: 
**********************************************************************
*��      ʷ: 
*1. <�޸���>     <�޸�����>     <�޸�˵��>

*********************************************************************/

#ifndef __CONTROL_H
#define __CONTROL_H

/*----------------------------------------------------
                     ͷ�ļ�
 ----------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "cmd_queue.h" 
#include <math.h>

/*----------------------------------------------------
                     �궨��
 ----------------------------------------------------*/
//#define Power_test

#define Res01 		1
#define Res02 		2
#define Res03 		3
#define Res04 		4
#define Res05 		5
#define Res06 		6
#define Res07 		7
#define Res08 		8
#define Res09     9
#define Res10     10

/*----------------------------------------------------
                     ��������
 ----------------------------------------------------*/
u16 MidFilter(u16* Array,u8 num );
u16 AverageFilter(u16* Array,u8 num );
u16 MidAveFilter(u16* Array,u8 num);
void Phase_ValueFilter(u8 num);
void ADC1_ValueFilter(u8 num);
void Send_Cmd(void);
void Impandence_Switch(void);
u16 Sweep(u32 Start_Fre,u32 End_Fre,u16 DAC_Value);
void PhaseLock(u32 Start_Fre,u32 End_Fre,u16 Voltage);
int CampareandAlarm(double num1,double num2,double num3,double num4,
 				double num5,double num6,double num7,double num8,double num9);

void USART2_printf (char *fmt,...);
void Send_Data_USB(void);
void Send_Data_PC(void);


//extern    u8 PhaseLock_OK;
extern volatile u16 CaptureNumber1;
extern s16 angle;
extern __IO uint16_t ADCConvertedValue[2];

extern u32 Time_100Ms;
extern u32 Time_100Ms_2;
extern s32 Capture_space;
//extern u32 P;
extern u16 Stop_Control_Flag;
extern uint16 Display_Mode_Flag;    //��ʾģʽ��־

extern u8 cmd_buffer[1024];  //����������ջ����� 
extern u8 USART2_RX;

extern void Delayus(__IO uint32_t nCount);
extern void Message_Deal(qsize size);

// С������
#define USART_REC_LEN  			10000  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
//extern UINT8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u32 USART_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
void uart2_init(u32 bound);
void USART2_printf (char *fmt,...);
void chart(uint16 display_flag);
#endif


