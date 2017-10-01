/*********************************************************************
*                Copyright (C), 2015-2016, Supersonics. Co., Ltd.
*                        �迹��������������
*
*                          Ӳ��ƽ̨: xxx
*                          �� о Ƭ: STM32F103
*                          �� Ŀ ��: xxx
**********************************************************************
*�ļ���: main.h
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

#ifndef _MAIN_H_
#define _MAIN_H_

#include "stm32f10x.h"
#include "cmd_queue.h"
#include "CtrFile.h"
#include "AD9833.h"

/*********************************************************************
                             �궨��
**********************************************************************/
#define DR_ADDRESS ((uint32_t)0x4001244C)     /*ADC1 DR�Ĵ�������ַ         */

/*********************************************************************
                             ��������
**********************************************************************/
void Delayus(__IO uint32_t kCount);
void RCC_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
void DMA_Configuration(void);
void ADC_Configuration(void);
void NVIC_Configuration(void);
void TIM_Configuration(void);
void DAC_Configuration(void);
void CPU_Init(void);

extern uint8 cmd_buffer[CMD_MAX_BUFFER];
extern void Message_Deal(qsize  size);
extern u16 count;//   DGUT
extern u16 count0; 


#endif
