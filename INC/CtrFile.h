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
extern uint16 SaveData_Ok_Flag;           //DGUT




extern u32 start_fre;                       /*定义起始频率   */
extern u32 end_fre;                         /*定义终止频率   */
extern u32 dac_value;                       /*定义DAC初值    */
extern u32 file_name;                   //DGUT定义文件名初值
extern u32 number_menu;                     /*定义显示菜单选项号码*/

/*****液晶屏定义变量*****/
extern double xiezhen_minfreq;                       /*定义谐振最小频率   */
extern double xiezhen_maxfreq;                         /*定义谐振最大频率   */
extern double fanxiezhen_minfreq;                       /*定义反谐振最小频率   */
extern double fanxiezhen_maxfreq;                         /*定义反谐振最大频率   */

extern double dongtai_minresis;                       /*定义动态最小电阻   */
extern double dongtai_maxresis;                         /*定义动态最大电阻   */

extern double jingtai_mincapac;                       /*定义静态最小电容*/
extern double jingtai_maxcapac;                         /*定义静态最大电容   */
extern double ziyou_mincapac;                       /*定义自由最小电容*/
extern double ziyou_maxcapac;                         /*定义自由最大电容   */
extern double dongtai_mincapac;                       /*定义动态最小电容*/
extern double dongtai_maxcapac;                         /*定义动态最大电容   */
extern double dongtai_minprod;                       /*定义动态最小电感*/
extern double dongtai_maxprod;                         /*定义动态最大电感   */


extern double fanxiezhen_minzukang;                       /*定义最小反谐振阻抗   */
extern double fanxiezhen_maxzukang;                         /*定义最大反谐振阻抗*/
extern double pinzhiyinshu_min;                       /*定义最小品质因数   */
extern double pinzhiyinshu_max;                         /*定义最大品质因数*/



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
