#ifndef _USER_UART__
#define _USER_UART__

#include "stm32f10x_it.h"     //�����û�MCU�����޸�

#define CMD_MAX_BUFFER 1024                           

#define uchar    unsigned char
#define uint8    unsigned char
#define uint16   unsigned short int
#define uint32   unsigned long

#define int16    short int

/****************************************************************************
* ��    �ƣ� UartInit()
* ��    �ܣ� ���ڳ�ʼ��
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
//void UartInit(uint32 Baudrate);

/*****************************************************************
* ��    �ƣ� SendChar()
* ��    �ܣ� ����1���ֽ�
* ��ڲ����� t  ���͵��ֽ�
* ���ڲ����� ��
 *****************************************************************/
void  SendChar(uchar t);

/*****************************************************************
* ��    �ƣ� SendStrings()
* ��    �ܣ� �����ַ���
* ��ڲ����� str  ���͵��ַ���
* ���ڲ����� ��
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
