/*******************************************************************************
* File        : CH375.h
* Author      : 
* Date        : 
* Description : CH375 底层驱动函数头文件
* Version     : V1.0

注：硬件连接部分有更新！！！

硬件连接：STM32F103R8T6      CH375
   	        PC0-PC7  <--->  D0-D7 数据端口
	            PC9  <----  INT#  在复位完成后为中断请求输出，低电平有效
                PC12  ---->  A0    命令/数据选择
	            PC8  ---->  CS#   片选信号
		        PC10  ---->  WR#   写选通
		        PC11  ---->  RD#   读选通
*******************************************************************************/
#ifndef  _CH375_H_
#define  _CH375_H_

/* Includes ---------------------------------------------------------------*/

#include"CH375INC.h"  //CH375相关命令及寄存器定义
#include"stm32f10x.h" //stm32f103r8t6的外设头文件
#include "sys.h"
//#include "usart.h"

/*端口引脚定义-------------------------------------------------------------*/

/* CH375 的数据端口*/
#define GPIO_CH375_Data  GPIOC
#define RCC_APB2Periph_GPIO_CH375_Data    RCC_APB2Periph_GPIOC

/* CH375 的数据引脚（双向）模式命令定义  PC0-PC7 */
#define DATA_MODE_IN   GPIOC->CRL = 0x44444444 //GPIOC端口定义为输入:IN_FLOATING 
#define DATA_MODE_OUT  GPIOC->CRL = 0x33333333 //GPIOC端口定义为输出:Out_PP,50Hz 


/* CH375 的控制端口*/ 
#define GPIO_CH375_CTL   GPIOC
#define RCC_APB2Periph_GPIO_CH375_CTL     RCC_APB2Periph_GPIOC

/* 命令/数据模式选择 A0: 1-写命令； 0-写数据*/
#define A0_Hx  GPIO_SetBits(GPIOC,GPIO_Pin_12)
#define A0_Lx GPIO_ResetBits(GPIOC,GPIO_Pin_12)

/* 片选 CS#  低电平有效*/ 
#define CS_Hx  GPIO_SetBits(GPIOC,GPIO_Pin_8)
#define CS_Lx  GPIO_ResetBits(GPIOC,GPIO_Pin_8)

/* 写选通 WR# 低电平有效*/
#define WR_Hx  GPIO_SetBits(GPIOC,GPIO_Pin_10)
#define WR_Lx  GPIO_ResetBits(GPIOC,GPIO_Pin_10)

/* 读选通 RD# 低电平有效*/
#define RD_Hx  GPIO_SetBits(GPIOC,GPIO_Pin_11)
#define RD_Lx  GPIO_ResetBits(GPIOC,GPIO_Pin_11)

/* Function Prototype ---------------------------------------------------*/


void CH375_DelayNus(__IO uint32_t nCount);  // CH375操作时需要的延时函数（根据不同的CPU需要调整） 

void CH375_Configuration(void);        // CH375 对应时钟，引脚的配置
 
void CH375_WriteCmd(uint8_t cmd);      // 向CH375写入命令
void CH375_WriteDat(uint8_t dat);      // 向CH375写入数据
uint8_t CH375_ReadCmd(void);           // 从CH375读取命令
uint8_t CH375_ReadDat(void);           // 从CH375读取数据或状态
 
uint8_t CH375_WaitInterrupt(void);     // 主机等待CH375芯片内部操作完成并产生中断，返回操作状态
 
uint8_t CH375_Init(void);              // 初始化CH375芯片
uint8_t CH375_DiskConnect(void);       // 磁盘是否连接
uint8_t CH375_DiskInit(void);          // 初始化磁盘
uint8_t CH375_DiskReady(void);         // 磁盘是否准备好（是够已经被初始化并得到分配地址）

uint8_t CH375_WriteSector(uint32_t addr, const uint8_t *pbuff); // 向U盘一扇区写入数据
uint8_t CH375_ReadSector(uint32_t addr, uint8_t *pbuff); // 从U盘一扇区读出数据

#endif   /*  _CH375_H_  */
 

