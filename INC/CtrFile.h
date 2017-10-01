/*********************************************************************
*                Copyright (C), 2015-2016, Supersonics. Co., Ltd.
*                        �迹��������������
*
*                          Ӳ��ƽ̨: xxx
*                          �� о Ƭ: STM32F103
*                          �� Ŀ ��: xxx
**********************************************************************
*�ļ���: CtrFile.h
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

#ifndef _CtrFile_H_
#define _CtrFile_H_

#include "stm32f10x.h"
#include "cmd_queue.h"

// ����RGBɫֵ
#define   BLACK		0x0000          /* ��ɫ��    0,   0,   0 */
#define   NAVY		0x000F          /* ����ɫ��  0,   0, 128 */
#define   DGREEN	0x03E0		      /* ����ɫ��  0, 128,   0 */
#define   DCYAN		0x03EF		      /* ����ɫ��  0, 128, 128 */
#define   MAROON	0x7800		      /* ���ɫ��  128, 0,   0 */
#define   PURPLE	0x780F		      /* ��ɫ��    128, 0, 128 */
#define   OLIVE		0x7BE0 	        /* ����̣�  128, 128, 0 */
#define   LGRAY		0xC618	        /* �Ұ�ɫ��  192, 192, 192 */
#define   DGRAY		0x7BEF		      /* ���ɫ��  128, 128, 128 */
#define   BLUE		0x001F		      /* ��ɫ��    0,   0, 255 */
#define   GREEN		0x07E0		      /* ��ɫ��    0, 255,   0 */
#define   CYAN	        0x07FF 		/* ��ɫ��    0, 255, 255 */
#define   RED		0xF800		        /* ��ɫ��    255,  0,  0 */
#define   MAGENTA	0xF81F		      /* Ʒ�죺    255, 0, 255 */
#define   YELLOW	0xFFE0		      /* ��ɫ��    255, 255, 0 */
#define   WHITE		0xFFFF          /* ��ɫ��    255, 255, 255 */
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
