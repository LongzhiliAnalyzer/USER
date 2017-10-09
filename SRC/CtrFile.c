/*********************************************************************
*                Copyright (C), 2015-2016, Supersonics. Co., Ltd.
*                        阻抗分析仪驱动程序
*
*                          硬件平台: xxx
*                          主 芯 片: STM32F103
*                          项 目 组: xxx
**********************************************************************
*文件名: CtrFile.c
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

#include "CtrFile.h"

uint16 screen_id;                        /*页面ID编号变量   */
uint16 control_id;                       /*控件ID编号变量   */
volatile uint16 current_screen_id;       /*当前屏幕处在的画面编号变量   */

/************************定义各控件标志位*****************************/
uint16 Ok_Control_Flag = 0;
uint16 Stop_Control_Flag = 0;
uint16 Clear_Flag = 0;
uint16 SaveData_Flag = 0;
uint16 SavePic_Flag = 0;
uint16 StarFre_Control_Flag = 0;
uint16 EndFre_Control_Flag = 0;
uint16 Dac_Control_Flag = 0;
uint16 Ok1_Control_Flag = 0;     //DGUT

u32 start_fre = 0;                       /*定义起始频率   */
u32 end_fre = 0;                         /*定义终止频率   */


/*****液晶屏定义变量*****/
u32 min_freq = 0;                       /*定义起始频率   */
u32 max_fre = 0;                         /*定义终止频率   */
u32 min_resis = 0;                       /*定义最小电阻   */
u32 max_resis = 0;                         /*定义最大电阻   */
u32 min_capac = 0;                       /*定义最小电容*/
u32 max_capac = 0;                         /*定义最大电容   */
u32 min_induc = 0;                       /*定义最小电感   */
u32 max_indut = 0;                         /*定义最大电感   */


u32 dac_value = 0;                       /*定义DAC初值    */
u32 file_name=0;         //DGUT


/*********************************************************************
* 函数作用：初始化液晶屏UI
* 函数参数：无
* 函数返回值：无
**********************************************************************/
void iniuserctr(void)
{
	int i = 9;
  /*修改文本框显示内容*/
  SetTextValue( 0, 6,"");                   /*         */  
  SetTextValue( 0, 7,"");                   /*         */  
  SetTextValue( 0, 8,"100");                /*         */  
  SetTextValue( 0, 25,"0");                 /*         */ 
	for(i=9; i<22; i++)
	SetTextValue(0, i, "");
	AnimationPlayFrame(0,1,1);
	AnimationPlayFrame(0,2,0);
	SetProgressValue(0, 24, 0);
}

/**************************************************************************************************************
* 名    称： OnRecvChangeScreen
* 功    能： 画面切换消息响应函数
* 入口参数： screen_id-切换后的画面ID
***************************************************************************************************************/
void OnRecvChangeScreen(uint16_t screen_id)
{
}

/**************************************************************************************************************
* 名    称： OnRecvTouchXY
* 功    能： 触摸坐标接收响应
* 入口参数： press-1按下触摸屏，3松开触摸屏
* 入口参数： x-触摸X坐标
* 入口参数： y-触摸y坐标
***************************************************************************************************************/
void OnRecvTouchXY(uint8_t press,uint16_t x,uint16_t y)
{
    //TODO: 用户代码
}

/**************************************************************************************************************
* 名    称： OnRecvButton
* 功    能： 按钮消息响应函数
* 入口参数： msg-指令缓冲区，size-指令大小
***************************************************************************************************************/
void OnRecvButton(PCTRL_MSG msg,qsize size)
{
	int i = 0;
  screen_id = PTR2U16(&msg->screen_id);     /*获取画面ID   */
  control_id = PTR2U16(&msg->control_id);   /*获取控件ID   */

  /**************************/
  /*-- SCREEN 0的画面处理 --*/
  /**************************/
  if(screen_id == 0 && control_id == 3)     /*"确定"的按键被按下了   */
  {
		Ok_Control_Flag = 1;
		AnimationPlayFrame(0,2,1);
		GraphChannelDataClear(0,23,0);
		GraphChannelDataClear(0,33,0);

		Delayus(100000);
		GraphChannelAdd(0,23,0,BLUE);           /*添加相位曲线通道   */
		GraphChannelAdd(0,33,0,RED);            /*添加阻抗曲线通道   */

		GetControlValue(0,6);				            /*获取起始频率   */
		GetControlValue(0,7);				            /*获取终止频率   */
		GetControlValue(0,8);				            /*获取DAC的数值	 */	
  }
	else if(screen_id == 0 && control_id == 4)	/*"停止"的案件被按下   */
	{
		Stop_Control_Flag = 1;
		AnimationPlayFrame(0,2,0);	
	}
	else if(screen_id == 0 && control_id == 26)
	{
		
	}
	else if(screen_id == 0 && control_id == 27)
	{
		Clear_Flag = 1;
	  SetTextValue( 0, 6,"");     
	  SetTextValue( 0, 7,"");  
    SetTextValue( 1, 2,"");	
	  SetTextValue( 0, 25,"0"); 
	  SetTextValue( 0, 31,"");     
	  SetTextValue( 0, 32,"");     
	  SetTextValue( 0, 34,"");    
	  SetTextValue( 0, 35,"");   
		for(i=9; i<22; i++)
		{
			SetTextValue(0, i, "");
		}
		AnimationPlayFrame(0,1,1);
		AnimationPlayFrame(0,2,0);
		SetProgressValue(0, 24, 0);
		GraphChannelDataClear(0,23,0);
		GraphChannelDataClear(0,33,0);
	}
	else if(screen_id == 0 && control_id == 28)
	{
		SaveData_Flag = 1;	
		SetProgressValue(0, 24, 0);
		SetTextValue( 0, 25,"0");
	}
	else if(screen_id == 0 && control_id == 29)
	{
		SavePic_Flag = 1;
	}
	//DGUT
	else if(screen_id == 1 && control_id == 3)
	{
		Ok1_Control_Flag = 1;
	}
//		else if(screen_id == 3 && control_id == 1)
//	{
//		back_Control_Flag = 1;
//	}
	
}

/**************************************************************************************************************
* 名    称： OnRecvText
* 功    能： 文本控件消息响应函数
* 入口参数： msg-指令缓冲区，size-指令大小
***************************************************************************************************************/
void OnRecvText(PCTRL_MSG msg, qsize size)
{
	uint32 i = 0;
  screen_id = PTR2U16(&msg->screen_id);         /*获取画面ID */
  control_id = PTR2U16(&msg->control_id);       /*获取控件ID */

  if(screen_id == 0 &&  control_id == 6)        /*获取系统自带键盘输入 */
  {
		if(Ok_Control_Flag == 1)
		{
			StarFre_Control_Flag = 1;
		}
		else
		{
			StarFre_Control_Flag = 0;
		}
		start_fre = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			start_fre = start_fre*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
  }
	else if(screen_id == 0 &&  control_id == 7)
	{
		if(Ok_Control_Flag == 1)
		{
			EndFre_Control_Flag = 1;
		}
		else
		{
			EndFre_Control_Flag = 0;
		}
		end_fre = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			end_fre = end_fre*10+ (((uint8 *)(&msg->param))[i++] - 0x30);      /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字 */
		}
	}
	else if(screen_id == 0 && control_id == 8)
	{
		if(Ok_Control_Flag == 1)
		{
			Dac_Control_Flag = 1;
		}
		else
		{
			Dac_Control_Flag = 0;
		}
		dac_value = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			dac_value = dac_value*10+ (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字 */
		}
	}//****DGUT
	
	else if(screen_id == 1 &&  control_id == 2)
	{
		file_name = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			file_name= file_name*10+ (((uint8 *)(&msg->param))[i++] -0x30);      /*从接收缓冲区取出键盘输入的数字（文件名），再转换成十进制数字 */		}
	}//**DGUT

	
	else if(screen_id == 6 &&  control_id == 3)				/*取得频率的最小值设定*/
	{
		min_freq = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			min_freq = min_freq*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 6 &&  control_id == 4)				/*取得频率的最大值设定*/
	{
		
		max_fre = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			max_fre = max_fre*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	
	}


	else if(screen_id == 6 &&  control_id == 5)					/*取得电阻的最小值设定*/
	{
		
		min_resis = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			min_resis = min_resis*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 6 &&  control_id == 6)							/*取得电阻的最大值设定*/
	{
		
		max_resis = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			max_resis = max_resis*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}



	else if(screen_id == 6 &&  control_id == 7)						/*取得电容的最小值设定*/
	{
		min_capac = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			min_capac = min_capac*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 6 &&  control_id == 8)						/*取得电容的最大值设定*/
	{
		
		max_capac = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			max_capac = max_capac*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
		else if(screen_id == 6 &&  control_id == 8)						/*取得电感的最小值设定*/
	{
		
		min_induc = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			min_induc = min_induc*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
			else if(screen_id == 6 &&  control_id == 8)						/*取得电感的最大值设定*/
	{
		max_indut = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			max_indut = max_indut*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
}

/**************************************************************************************************************
* 名    称： OnRecvProgress
* 功    能： 进度条消息响应函数
* 入口参数： msg-指令缓冲区，size-指令大小
***************************************************************************************************************/
void OnRecvProgress(PCTRL_MSG msg,qsize size)
{
// 	uint32 value = PTR2U32(msg->param);     //数值
// 	screen_id = PTR2U16(&msg->screen_id);   //画面ID
// 	control_id = PTR2U16(&msg->control_id); //控件ID
    //TODO: 用户代码
}

/**************************************************************************************************************
* 名    称： OnRecvSlider
* 功    能： 滑动条消息响应函数
* 入口参数： msg-指令缓冲区，size-指令大小
***************************************************************************************************************/
void OnRecvSlider(PCTRL_MSG msg,qsize size)
{
	;
}

/**************************************************************************************************************
* 名    称： OnRecvMeter
* 功    能： 仪表消息响应函数
* 入口参数： msg-指令缓冲区，size-指令大小
***************************************************************************************************************/
void OnRecvMeter(PCTRL_MSG msg,qsize size)
{
	;
}

/**************************************************************************************************************
* 名    称： OnRecvMeter
* 功    能： 图标控件响应函数
* 入口参数： msg-指令缓冲区，size-指令大小
***************************************************************************************************************/
void OnRecvAnimation(PCTRL_MSG msg,qsize size)
{
//     uint16 screen_id = PTR2U16(&msg->screen_id);  //画面ID
//     uint16 control_id = PTR2U16(&msg->control_id);//控件ID
//     uint32 value = PTR2U32(msg->param);           
	
    //TODO: 用户代码
}

/**************************************************************************************************************
* 名    称： OnRecCurrentScreen
* 功    能： 画面状态响应函数
* 入口参数： msg-指令缓冲区，size-指令大小
***************************************************************************************************************/
void	OnRecCurrentScreen(PCTRL_MSG msg,qsize size)
{
	 current_screen_id = PTR2U16(&msg->screen_id);
}

/*****************************以上是按钮控件的消息处理函数*********************************/
