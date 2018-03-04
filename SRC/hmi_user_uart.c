/************************************版权申明********************************************
**                             广州大彩光电科技有限公司                                
**                             http://www.gz-dc.com
**-----------------------------------文件信息--------------------------------------------
** 文件名称:   hmi_user_uart.c 
** 修改时间:   2011-05-18
** 文件说明:   用户MCU串口驱动函数库
** 技术支持：  Tel: 020-22077476  Email: hmi@gz-dc.com  
**--------------------------------------------------------------------------------------*/
        
/**--------------------------------------------------------------------------------------    
                                  使用必读                      
   hmi_user_uart.c中的串口发送接收函数共3个函数：串口初始化Uartinti()、发送1个字节SendChar()、
   发送字符串SendStrings().目前暂时只提供8051驱动平台，如果用户MCU非8051平台，需要修改底层寄
   存器设置,但禁止修改函数名称，否则无法与HMI驱动库(hmi_driver.c)匹配。 
**--------------------------------------------------------------------------------------*/

#include "hmi_user_uart.h"
#include "cmd_queue.h"
#include "CtrFile.h"
#include "control.h"
#include <stdio.h>
#include "stm32f10x.h"
#include "main.h"

 /*
 串口屏返回一条命令的最长长度，在RAM允许的情况下，尽可能设大一些，
 具体指令返回长度请参照指令手册（各系列的串口屏规格书中有附）串口屏能支持的一条命令最大长度是2048字节
*/
uint8 cmd_buffer[CMD_MAX_BUFFER];
unsigned char Buff[] = {0};	

//uint16 SaveData_Ok_Flag = 0;     //DGUT

/*----------------------------------------------------------------------------------------
**                          1. 基于8051平台串口驱动
*----------------------------------------------------------------------------------------*/

/****************************************************************************
* 名    称： UartInit()
* 功    能： 串口初始化
* 入口参数： 无      
* 出口参数： 无
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
//	USART_DeInit(USART1);//复位USART1	  
//	USART_InitStructure.USART_BaudRate = BaudRate;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode =   USART_Mode_Tx|USART_Mode_Rx;
//
//	/* USART configuration */
//	USART_Init(USART1, &USART_InitStructure);
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //接收中断使能
//
//	/* Enable USART */
//	USART_Cmd(USART1, ENABLE);   
//  }
/*****************************************************************
* 名    称： SendChar()
* 功    能： 发送1个字节
* 入口参数： t  发送的字节       
* 出口参数： 无                  
 *****************************************************************/
void  SendChar(uchar t) 
 {
		USART_SendData(USART1,t);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);         
 } 
/*****************************************************************
* 名    称： SendStrings()
* 功    能： 发送字符串
* 入口参数： str  发送的字符串     
* 出口参数： 无               
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
* 函数作用：液晶屏消息处理函数
* 函数参数：消息长度
* 函数返回值：无
******************************************************************************/
void Message_Deal(qsize  size)
{
	uint8 cmd_type = 0;
	uint8 msg_type = 0;
  uint8 control_type = 0;
  PCTRL_MSG msg = 0;

  msg = (PCTRL_MSG)cmd_buffer;
	cmd_type = msg->cmd_type;
  /*根据接收到的串口屏命令返回，转到对应的分支去处理事件*/
  switch(cmd_type)                                   /*判断指令类型   */
  {
		case CMD_TOUCH_PRESS_RETURN:                     /*按下触摸屏坐标上传   */
			break;
    case CMD_TOUCH_RELEASE_RETURN:                   /*松开触摸屏坐标上传   */
			//OnRecvTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4));
			break;

    case CMD_TYPE_UPDATE_CONTROL:                    /*控件更新消息         */
			msg_type = msg->ctrl_msg;
      switch(msg_type)                               /*判断消息类型         */
      {   
				case kCtrlMsgGetCurrentScreen:					
					OnRecCurrentScreen(msg,size);              /*获取当前画面值       */
					break;
				case kCtrlMsgGetData:
					control_type = msg->control_type;          /*获取控件类型         */
					switch(control_type)
					{
						case kCtrlMsgGetCurrentScreen:
							OnRecCurrentScreen(msg,size);          /*更新当前画面值       */
						case kCtrlButton:
							OnRecvButton(msg,size);                /*按钮控件处理         */
							break; 
						case kCtrlText:
							OnRecvText(msg,size);                  /*文本控件处理         */
							break; 
						case kCtrlmenu:								/*菜单控件处理         */
							OnRecmenu(msg,size);
							break;
						case kCtrlProgress:
							//OnRecvProgress(msg,size);            /*进度条控件处理       */
							break; 
						case kCtrlSlider:                        /*滑动条控件处理       */
							//OnRecvSlider(msg,size);
							break; 
						case kCtrlMeter:                         /*仪表控件处理         */
							//OnRecvMeter(msg,size);
							break; 
						case kCtrlAnimation:                     /*图标或者动画控件处理 */
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
* 函数作用：检测液晶面板启动按钮是否按下
* 函数参数：无
* 函数返回值：无
**********************************************************************************/
void OK_Button(void)
{
	if((Ok_Control_Flag==1) && (StarFre_Control_Flag==1) && (EndFre_Control_Flag==1) && (Dac_Control_Flag==1))
	{
		Ok_Control_Flag = 0;
		StarFre_Control_Flag = 0;
		EndFre_Control_Flag = 0;
		Dac_Control_Flag = 0;
		AnimationStop(0,29);AnimationStop(0,26);   //停止超限指示灯闪烁
		if(end_fre > start_fre)
		{
			ShowControl(0,3,0);        //启动按钮
			ShowControl(0,27,0);       //清屏按钮
			ShowControl(0,4,1);        //停止按钮
			ShowControl(0,28,0);        //保存数据按钮
			GPIO_ResetBits(GPIOA,GPIO_Pin_7);	
			
			sprintf((char*)Buff,"%-7.0f",(double)start_fre);
			SetTextValue(0,31,Buff);   //图表起始频率
			sprintf((char*)Buff,"%-7.0f",(double)end_fre);
			SetTextValue(0,32,Buff);   //图表终止频率
			sprintf((char*)Buff,"%-7.0f",(double)start_fre);
			SetTextValue(0,34,Buff);   //图表起始频率
			sprintf((char*)Buff,"%-7.0f",(double)end_fre);
			SetTextValue(0,35,Buff);   //图表终止频率

			PhaseLock(start_fre*1000, end_fre*1000,(u16)dac_value);

			Ok_Control_Flag = 0;
			StarFre_Control_Flag = 0;
			EndFre_Control_Flag = 0;
			Dac_Control_Flag = 0;
			Stop_Control_Flag = 0;  
	
			ShowControl(0,3,1);     //启动按钮
			ShowControl(0,27,1);    //清屏按钮
			ShowControl(0,4,0);     //停止按钮
			ShowControl(0,28,0);    //保存数据按钮
			
			Beep_On();         //开蜂鸣器
			Delayus(400000);
			queue_reset();		 //清空串口接收缓冲区
			Beep_Off();        //关蜂鸣器
			GPIO_SetBits(GPIOA,GPIO_Pin_7);
		}
		ShowControl(0,28,1);    //保存数据按钮
	}
	ShowControl(0,3,1);       //启动按钮
	ShowControl(0,27,1);      //清屏按钮
}	

/*********************************************************************************
* 函数作用：检测液晶面板停止按钮是否按下
* 函数参数：无
* 函数返回值：无
**********************************************************************************/
void Stop_Button(void)
{
	if(Stop_Control_Flag == 1)
	{ 
		Stop_Control_Flag = 0;
		queue_reset();					        /*清空串口接收缓冲区   */
		Delayus(100000);	
	}
}
/*********************************************************************************
* 函数作用：检测液晶面板清屏按钮是否按下
* 函数参数：无
* 函数返回值：无
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
		AnimationStop(0,29);AnimationStop(0,26);  //停止超限指示灯闪烁
		Delayus(100000);
	}
}
/*********************************************************************************
* 函数作用：检测液晶面板保存数据按钮是否按下
* 函数参数：无
* 函数返回值：无
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
* 函数作用：检测液晶面板保存图片按钮是否按下
* 函数参数：无
* 函数返回值：无
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

