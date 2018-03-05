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
extern uint16 SaveData_Ok_Flag;           //DGUT




extern u32 start_fre;                       /*������ʼƵ��   */
extern u32 end_fre;                         /*������ֹƵ��   */
extern u32 dac_value;                       /*����DAC��ֵ    */
extern u32 file_name;                   //DGUT�����ļ�����ֵ
extern u32 number_menu;                     /*������ʾ�˵�ѡ�����*/

/*****Һ�����������*****/
extern double xiezhen_minfreq;                       /*����г����СƵ��   */
extern double xiezhen_maxfreq;                         /*����г�����Ƶ��   */
extern double fanxiezhen_minfreq;                       /*���巴г����СƵ��   */
extern double fanxiezhen_maxfreq;                         /*���巴г�����Ƶ��   */

extern double dongtai_minresis;                       /*���嶯̬��С����   */
extern double dongtai_maxresis;                         /*���嶯̬������   */

extern double jingtai_mincapac;                       /*���徲̬��С����*/
extern double jingtai_maxcapac;                         /*���徲̬������   */
extern double ziyou_mincapac;                       /*����������С����*/
extern double ziyou_maxcapac;                         /*��������������   */
extern double dongtai_mincapac;                       /*���嶯̬��С����*/
extern double dongtai_maxcapac;                         /*���嶯̬������   */
extern double dongtai_minprod;                       /*���嶯̬��С���*/
extern double dongtai_maxprod;                         /*���嶯̬�����   */


extern double fanxiezhen_minzukang;                       /*������С��г���迹   */
extern double fanxiezhen_maxzukang;                         /*�������г���迹*/
extern double pinzhiyinshu_min;                       /*������СƷ������   */
extern double pinzhiyinshu_max;                         /*�������Ʒ������*/



void iniuserctr(void);
void OnRecvChangeScreen(uint16_t screen_id);
void OnRecvTouchXY(uint8_t press,uint16_t x,uint16_t y);
void OnRecvButton(PCTRL_MSG msg,qsize size);
void OnRecvText(PCTRL_MSG msg,qsize size);
void OnRecvProgress(PCTRL_MSG msg,qsize size);
void OnRecvSlider(PCTRL_MSG msg,qsize size);
void OnRecvMeter(PCTRL_MSG msg,qsize size);
void OnRecvAnimation(PCTRL_MSG msg,qsize size);
void OnRecCurrentScreen(PCTRL_MSG msg,qsize size);
void OnRecmenu(PCTRL_MSG msg,qsize size);

extern void Delayus(__IO uint32_t kCount);
extern void OnRecvChangeScreen(uint16_t screen_id);
extern void chart(uint16 display_flag);

#endif
