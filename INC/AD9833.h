/*********************************************************************
*                Copyright (C), 2015-2016, Supersonics. Co., Ltd.
*                        阻抗分析仪驱动程序
*
*                          硬件平台: xxx
*                          主 芯 片: STM32F103
*                          项 目 组: xxx
**********************************************************************
*文件名: AD9833.h
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

#ifndef _AD9833_H_
#define _AD9833_H_

#include "stm32f10x.h"

#define FMCLK  16000000            /* 晶振频率 */

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
