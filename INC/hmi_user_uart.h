#ifndef _USER_UART__
#define _USER_UART__

#include "stm32f10x_it.h"     //根据用户MCU进行修改

#define CMD_MAX_BUFFER 1024                           

#define uchar    unsigned char
#define uint8    unsigned char
#define uint16   unsigned short int
#define uint32   unsigned long

#define int16    short int

/****************************************************************************
* 名    称： UartInit()
* 功    能： 串口初始化
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
//void UartInit(uint32 Baudrate);

/*****************************************************************
* 名    称： SendChar()
* 功    能： 发送1个字节
* 入口参数： t  发送的字节
* 出口参数： 无
 *****************************************************************/
void  SendChar(uchar t);

/*****************************************************************
* 名    称： SendStrings()
* 功    能： 发送字符串
* 入口参数： str  发送的字符串
* 出口参数： 无
 *****************************************************************/
void SendStrings(uchar *str);
void OK_Button(void);
void Stop_Button(void);
void ClearScreen_Button(void);
void SaveData_Button(void);
void SavePic_Button(void);


extern uint16 Ok_Control_Flag;
extern uint16 Stop_Control_Flag;
extern uint16 Clear_Flag;
extern uint16 SaveData_Flag;
extern uint16 SavePic_Flag;
extern uint16 StarFre_Control_Flag;
extern uint16 EndFre_Control_Flag;
extern uint16 Dac_Control_Flag;


extern void Delayus(__IO uint32_t kCount);


#endif
