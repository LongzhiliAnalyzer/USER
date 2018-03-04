/************************************��Ȩ����********************************************
**                             ���ݴ�ʹ��Ƽ����޹�˾                                
**                             http://www.gz-dc.com
**-----------------------------------�ļ���Ϣ--------------------------------------------
** �ļ�����:   hmi_user_uart.c 
** �޸�ʱ��:   2011-05-18
** �ļ�˵��:   �û�MCU��������������
** ����֧�֣�  Tel: 020-22077476  Email: hmi@gz-dc.com  
**--------------------------------------------------------------------------------------*/
        
/**--------------------------------------------------------------------------------------    
                                  ʹ�ñض�                      
   hmi_user_uart.c�еĴ��ڷ��ͽ��պ�����3�����������ڳ�ʼ��Uartinti()������1���ֽ�SendChar()��
   �����ַ���SendStrings().Ŀǰ��ʱֻ�ṩ8051����ƽ̨������û�MCU��8051ƽ̨����Ҫ�޸ĵײ��
   ��������,����ֹ�޸ĺ������ƣ������޷���HMI������(hmi_driver.c)ƥ�䡣 
**--------------------------------------------------------------------------------------*/

#include "hmi_user_uart.h"
#include "cmd_queue.h"
#include "CtrFile.h"
#include "control.h"
#include <stdio.h>
#include "stm32f10x.h"
#include "main.h"

 /*
 ����������һ�����������ȣ���RAM���������£����������һЩ��
 ����ָ��س��������ָ���ֲᣨ��ϵ�еĴ�������������и�����������֧�ֵ�һ��������󳤶���2048�ֽ�
*/
uint8 cmd_buffer[CMD_MAX_BUFFER];
unsigned char Buff[] = {0};	

//uint16 SaveData_Ok_Flag = 0;     //DGUT

/*----------------------------------------------------------------------------------------
**                          1. ����8051ƽ̨��������
*----------------------------------------------------------------------------------------*/

/****************************************************************************
* ��    �ƣ� UartInit()
* ��    �ܣ� ���ڳ�ʼ��
* ��ڲ����� ��      
* ���ڲ����� ��
****************************************************************************/
//void UartInit(uint16_t BaudRate)
//{ 
//	GPIO_InitTypeDef        GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	/* Enable GPIO clock */
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
//
//	/* Configure USART Tx as alternate function push-pull */
//	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//	//  /* Configure USART Rx as input floating */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	USART_DeInit(USART1);//��λUSART1	  
//	USART_InitStructure.USART_BaudRate = BaudRate;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode =   USART_Mode_Tx|USART_Mode_Rx;
//
//	/* USART configuration */
//	USART_Init(USART1, &USART_InitStructure);
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //�����ж�ʹ��
//
//	/* Enable USART */
//	USART_Cmd(USART1, ENABLE);   
//  }
/*****************************************************************
* ��    �ƣ� SendChar()
* ��    �ܣ� ����1���ֽ�
* ��ڲ����� t  ���͵��ֽ�       
* ���ڲ����� ��                  
 *****************************************************************/
void  SendChar(uchar t) 
 {
		USART_SendData(USART1,t);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);         
 } 
/*****************************************************************
* ��    �ƣ� SendStrings()
* ��    �ܣ� �����ַ���
* ��ڲ����� str  ���͵��ַ���     
* ���ڲ����� ��               
 *****************************************************************/

 void SendStrings(uchar *str)
 {
     while(*str)
	 {
	  SendChar(*str);
	  str++;
	 }
 } 
 
 /*****************************************************************************
* �������ã�Һ������Ϣ������
* ������������Ϣ����
* ��������ֵ����
******************************************************************************/
void Message_Deal(qsize  size)
{
	uint8 cmd_type = 0;
	uint8 msg_type = 0;
  uint8 control_type = 0;
  PCTRL_MSG msg = 0;

  msg = (PCTRL_MSG)cmd_buffer;
	cmd_type = msg->cmd_type;
  /*���ݽ��յ��Ĵ���������أ�ת����Ӧ�ķ�֧ȥ�����¼�*/
  switch(cmd_type)                                   /*�ж�ָ������   */
  {
		case CMD_TOUCH_PRESS_RETURN:                     /*���´����������ϴ�   */
			break;
    case CMD_TOUCH_RELEASE_RETURN:                   /*�ɿ������������ϴ�   */
			//OnRecvTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4));
			break;

    case CMD_TYPE_UPDATE_CONTROL:                    /*�ؼ�������Ϣ         */
			msg_type = msg->ctrl_msg;
      switch(msg_type)                               /*�ж���Ϣ����         */
      {   
				case kCtrlMsgGetCurrentScreen:					
					OnRecCurrentScreen(msg,size);              /*��ȡ��ǰ����ֵ       */
					break;
				case kCtrlMsgGetData:
					control_type = msg->control_type;          /*��ȡ�ؼ�����         */
					switch(control_type)
					{
						case kCtrlMsgGetCurrentScreen:
							OnRecCurrentScreen(msg,size);          /*���µ�ǰ����ֵ       */
						case kCtrlButton:
							OnRecvButton(msg,size);                /*��ť�ؼ�����         */
							break; 
						case kCtrlText:
							OnRecvText(msg,size);                  /*�ı��ؼ�����         */
							break; 
						case kCtrlmenu:								/*�˵��ؼ�����         */
							OnRecmenu(msg,size);
							break;
						case kCtrlProgress:
							//OnRecvProgress(msg,size);            /*�������ؼ�����       */
							break; 
						case kCtrlSlider:                        /*�������ؼ�����       */
							//OnRecvSlider(msg,size);
							break; 
						case kCtrlMeter:                         /*�Ǳ�ؼ�����         */
							//OnRecvMeter(msg,size);
							break; 
						case kCtrlAnimation:                     /*ͼ����߶����ؼ����� */
							//OnRecvAnimation(msg,size);
							break; 
						default:
							break;
					}
					break;
			}
			break;
	}
}

/*********************************************************************************
* �������ã����Һ�����������ť�Ƿ���
* ������������
* ��������ֵ����
**********************************************************************************/
void OK_Button(void)
{
	if((Ok_Control_Flag==1) && (StarFre_Control_Flag==1) && (EndFre_Control_Flag==1) && (Dac_Control_Flag==1))
	{
		Ok_Control_Flag = 0;
		StarFre_Control_Flag = 0;
		EndFre_Control_Flag = 0;
		Dac_Control_Flag = 0;
		AnimationStop(0,29);AnimationStop(0,26);   //ֹͣ����ָʾ����˸
		if(end_fre > start_fre)
		{
			ShowControl(0,3,0);        //������ť
			ShowControl(0,27,0);       //������ť
			ShowControl(0,4,1);        //ֹͣ��ť
			ShowControl(0,28,0);        //�������ݰ�ť
			GPIO_ResetBits(GPIOA,GPIO_Pin_7);	
			
			sprintf((char*)Buff,"%-7.0f",(double)start_fre);
			SetTextValue(0,31,Buff);   //ͼ����ʼƵ��
			sprintf((char*)Buff,"%-7.0f",(double)end_fre);
			SetTextValue(0,32,Buff);   //ͼ����ֹƵ��
			sprintf((char*)Buff,"%-7.0f",(double)start_fre);
			SetTextValue(0,34,Buff);   //ͼ����ʼƵ��
			sprintf((char*)Buff,"%-7.0f",(double)end_fre);
			SetTextValue(0,35,Buff);   //ͼ����ֹƵ��

			PhaseLock(start_fre*1000, end_fre*1000,(u16)dac_value);

			Ok_Control_Flag = 0;
			StarFre_Control_Flag = 0;
			EndFre_Control_Flag = 0;
			Dac_Control_Flag = 0;
			Stop_Control_Flag = 0;  
	
			ShowControl(0,3,1);     //������ť
			ShowControl(0,27,1);    //������ť
			ShowControl(0,4,0);     //ֹͣ��ť
			ShowControl(0,28,0);    //�������ݰ�ť
			
			Beep_On();         //��������
			Delayus(400000);
			queue_reset();		 //��մ��ڽ��ջ�����
			Beep_Off();        //�ط�����
			GPIO_SetBits(GPIOA,GPIO_Pin_7);
		}
		ShowControl(0,28,1);    //�������ݰ�ť
	}
	ShowControl(0,3,1);       //������ť
	ShowControl(0,27,1);      //������ť
}	

/*********************************************************************************
* �������ã����Һ�����ֹͣ��ť�Ƿ���
* ������������
* ��������ֵ����
**********************************************************************************/
void Stop_Button(void)
{
	if(Stop_Control_Flag == 1)
	{ 
		Stop_Control_Flag = 0;
		queue_reset();					        /*��մ��ڽ��ջ�����   */
		Delayus(100000);	
	}
}
/*********************************************************************************
* �������ã����Һ�����������ť�Ƿ���
* ������������
* ��������ֵ����
**********************************************************************************/
void ClearScreen_Button(void)
{
	if(Clear_Flag == 1)
	{		 
		Clear_Flag = 0;
		ShowControl(0,4,0);     
        ShowControl(0,27,0); 
		ShowControl(0,28,0);
		queue_reset();
		AnimationStop(0,29);AnimationStop(0,26);  //ֹͣ����ָʾ����˸
		Delayus(100000);
	}
}
/*********************************************************************************
* �������ã����Һ����屣�����ݰ�ť�Ƿ���
* ������������
* ��������ֵ����
**********************************************************************************/
void SaveData_Button(void)
{
	if(SaveData_Ok_Flag == 1)
 	{	    		
		SaveData_Ok_Flag=0;
		SaveData_Flag = 0;
        Clear_Flag = 0;
		Send_Data_USB();
//      Send_Data_PC();
		queue_reset();
		Delayus(100000);
	}
}
/*********************************************************************************
* �������ã����Һ����屣��ͼƬ��ť�Ƿ���
* ������������
* ��������ֵ����
**********************************************************************************/
void SavePic_Button(void)
{
	if(SavePic_Flag == 1)
	{
		SavePic_Flag = 0;
		queue_reset();
		Delayus(100000);
	}
}

