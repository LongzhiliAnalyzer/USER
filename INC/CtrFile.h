/*********************************************************************
*                Copyright (C), 2015-2016, Supersonics. Co., Ltd.
*                        阻抗分析仪驱动程序
*
*                          硬件平台: xxx
*                          主 芯 片: STM32F103
*                          项 目 组: xxx
**********************************************************************
*文件名: CtrFile.h
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

#ifndef _CtrFile_H_
#define _CtrFile_H_

#include "stm32f10x.h"
#include "cmd_queue.h"

// 定义RGB色值
#define   BLACK		0x0000          /* 黑色：    0,   0,   0 */
#define   NAVY		0x000F          /* 深蓝色：  0,   0, 128 */
#define   DGREEN	0x03E0		      /* 深绿色：  0, 128,   0 */
#define   DCYAN		0x03EF		      /* 深青色：  0, 128, 128 */
#define   MAROON	0x7800		      /* 深红色：  128, 0,   0 */
#define   PURPLE	0x780F		      /* 紫色：    128, 0, 128 */
#define   OLIVE		0x7BE0 	        /* 橄榄绿：  128, 128, 0 */
#define   LGRAY		0xC618	        /* 灰白色：  192, 192, 192 */
#define   DGRAY		0x7BEF		      /* 深灰色：  128, 128, 128 */
#define   BLUE		0x001F		      /* 蓝色：    0,   0, 255 */
#define   GREEN		0x07E0		      /* 绿色：    0, 255,   0 */
#define   CYAN	        0x07FF 		/* 青色：    0, 255, 255 */
#define   RED		0xF800		        /* 红色：    255,  0,  0 */
#define   MAGENTA	0xF81F		      /* 品红：    255, 0, 255 */
#define   YELLOW	0xFFE0		      /* 黄色：    255, 255, 0 */
#define   WHITE		0xFFFF          /* 白色：    255, 255, 255 */
enum {UNVISABLE,VISABLE};
extern uint16 Ok1_Control_Flag;           //DGUT

void iniuserctr(void);
void OnRecvChangeScreen(uint16_t screen_id);
void OnRecvTouchXY(uint8_t press,uint16_t x,uint16_t y);
void OnRecvButton(PCTRL_MSG msg,qsize size);
void OnRecvText(PCTRL_MSG msg,qsize size);
void OnRecvProgress(PCTRL_MSG msg,qsize size);
void OnRecvSlider(PCTRL_MSG msg,qsize size);
void OnRecvMeter(PCTRL_MSG msg,qsize size);
void OnRecvAnimation(PCTRL_MSG msg,qsize size);
void	OnRecCurrentScreen(PCTRL_MSG msg,qsize size);

extern void Delayus(__IO uint32_t kCount);
extern void OnRecvChangeScreen(uint16_t screen_id);

#endif
