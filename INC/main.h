/*********************************************************************
*                Copyright (C), 2015-2016, Supersonics. Co., Ltd.
*                        阻抗分析仪驱动程序
*
*                          硬件平台: xxx
*                          主 芯 片: STM32F103
*                          项 目 组: xxx
**********************************************************************
*文件名: main.h
*版  本: V1.0.0
*作  者: 
*日  期: 
*说  明:
**********************************************************************
*重要贡献者: 
**********************************************************************
*历      史: 
*1. <修改者>     <修改日期>     <修改说明>

*********************************************************************/

#ifndef _MAIN_H_
#define _MAIN_H_

#include "stm32f10x.h"
#include "cmd_queue.h"
#include "CtrFile.h"
#include "AD9833.h"

/*********************************************************************
                             宏定义
**********************************************************************/
#define DR_ADDRESS ((uint32_t)0x4001244C)     /*ADC1 DR寄存器基地址         */

/*********************************************************************
                             函数声明
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
