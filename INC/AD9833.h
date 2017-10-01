/*********************************************************************
*                Copyright (C), 2015-2016, Supersonics. Co., Ltd.
*                        �迹��������������
*
*                          Ӳ��ƽ̨: xxx
*                          �� о Ƭ: STM32F103
*                          �� Ŀ ��: xxx
**********************************************************************
*�ļ���: AD9833.h
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

#ifndef _AD9833_H_
#define _AD9833_H_

#include "stm32f10x.h"

#define FMCLK  16000000            /* ����Ƶ�� */

#define CS_L   GPIO_ResetBits(GPIOB,GPIO_Pin_11)
#define CS_H   GPIO_SetBits(GPIOB,GPIO_Pin_11)

#define SCL_L  GPIO_ResetBits(GPIOB,GPIO_Pin_13)
#define SCL_H  GPIO_SetBits(GPIOB,GPIO_Pin_13)

#define SDA_L  GPIO_ResetBits(GPIOB,GPIO_Pin_14)
#define SDA_H  GPIO_SetBits(GPIOB,GPIO_Pin_14)

void AD9833_Init(void);
void Write_word(u16 DATA);
void ad9833_out(u32 freq_value,u8 type);

#endif
