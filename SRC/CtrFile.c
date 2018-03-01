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
uint16 Display_Mode_Flag = 0;     //显示模式标志位

u32 start_fre = 0;                       /*定义起始频率   */
u32 end_fre = 0;                         /*定义终止频率   */


/*****液晶屏定义变量*****/
u32 xiezhen_minfreq = 0;                       /*定义谐振最小频率   */
u32 xiezhen_maxfreq = 0xFFFFFFFF;                         /*定义谐振最大频率   */
u32 fanxiezhen_minfreq = 0;                       /*定义反谐振最小频率   */
u32 fanxiezhen_maxfreq = 0xFFFFFFFF;                         /*定义反谐振最大频率   */

u32 dongtai_minresis = 0;                       /*定义动态最小电阻   */
u32 dongtai_maxresis = 0xFFFFFFFF;                         /*定义动态最大电阻   */

u32 jingtai_mincapac = 0;                       /*定义静态最小电容*/
u32 jingtai_maxcapac = 0xFFFFFFFF;                         /*定义静态最大电容   */
u32 ziyou_mincapac = 0;                       /*定义自由最小电容*/
u32 ziyou_maxcapac = 0xFFFFFFFF;                         /*定义自由最大电容   */
u32 dongtai_mincapac = 0;                       /*定义动态最小电容*/
u32 dongtai_maxcapac = 0xFFFFFFFF;                         /*定义动态最大电容   */
u32 dongtai_minprod = 0;                       /*定义动态最小电感*/
u32 dongtai_maxprod = 0xFFFFFFFF;                         /*定义动态最大电感   */


u32 fanxiezhen_minzukang = 0;                       /*定义最小反谐振阻抗   */
u32 fanxiezhen_maxzukang = 0xFFFFFFFF;                         /*定义最大反谐振阻抗*/
u32 pinzhiyinshu_min = 0;                       /*定义最小品质因数   */
u32 pinzhiyinshu_max = 0xFFFFFFFF;                         /*定义最大品质因数*/

u32 dac_value = 0;                       /*定义DAC初值    */
u32 file_name=0;         //DGUT
u32 number_menu = 0;                       /*定义显示菜单选项号码*/


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
		AnimationPlayFrame(0,26,0);
		AnimationPlayFrame(0,25,0);
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
	else if(screen_id == 0 && control_id == 29)//有疑问！！！
	{
		SavePic_Flag = 1;
	}
	//DGUT
	else if(screen_id == 1 && control_id == 3)
	{
		Ok1_Control_Flag = 1;
	}

	//设置曲线显示模式
	else if(screen_id == 14 && control_id == 2)//设置显示模式   1、频率__阻抗log  2、
	{
		 Display_Mode_Flag=0;
	}
	else if(screen_id == 14 && control_id == 3)//设置显示模式   1、频率——电阻     2、
	{
		Display_Mode_Flag=1;
	}
	else if(screen_id == 14 && control_id == 4)//设置显示模式
	{
		Display_Mode_Flag=2;
	}
	else if(screen_id == 14 && control_id == 5)//设置显示模式
	{
		Display_Mode_Flag=3;
	}
	else if(screen_id == 14 && control_id == 6)//设置显示模式
	{
		Display_Mode_Flag=4;
	}
	else if(screen_id == 14 && control_id == 7)//设置显示模式
	{
		Display_Mode_Flag=5;
	}
	
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

	
	else if(screen_id == 5 &&  control_id == 2)				/*取得谐振频率的最小值设定*/
	{
		xiezhen_minfreq = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			xiezhen_minfreq = xiezhen_minfreq*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 5 &&  control_id == 11)				/*取得谐振频率的最大值设定*/
	{
		
		xiezhen_maxfreq = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			xiezhen_maxfreq = xiezhen_maxfreq*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	
	}


	else if(screen_id == 5 &&  control_id == 3)					/*取得反谐振频率的最小值设定*/
	{
		
		fanxiezhen_minfreq = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			fanxiezhen_minfreq = fanxiezhen_minfreq*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 5 &&  control_id == 12)							/*取得反谐振频率的最大值设定*/
	{
		
		fanxiezhen_maxfreq = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			fanxiezhen_maxfreq = fanxiezhen_maxfreq*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}



	else if(screen_id == 5 &&  control_id == 4)						/*取得动态电阻的最小值设定*/
	{
		dongtai_minresis = 0;
    	while(((uint8 *)(&msg->param))[i])
		{
			dongtai_minresis = dongtai_minresis*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 5 &&  control_id == 13)						/*取得动态电阻的最大值设定*/
	{
		
		dongtai_maxresis = 0;
    	while(((uint8 *)(&msg->param))[i])
		{
			dongtai_maxresis = dongtai_maxresis*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 5 &&  control_id == 5)						/*取得静态电容的最小值设定*/
	{
		
		jingtai_mincapac = 0;
    	while(((uint8 *)(&msg->param))[i])
		{
			jingtai_mincapac = jingtai_mincapac*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 6 &&  control_id == 14)						/*取得静态电容的最大值设定*/
	{
		jingtai_maxcapac = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			jingtai_maxcapac = jingtai_maxcapac*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 5 &&  control_id == 6)						/*取得自由电容的最小值设定*/
	{
		ziyou_mincapac = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			ziyou_mincapac = ziyou_mincapac*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 5 &&  control_id == 15)						/*取得自由电容的最大值设定*/
	{
		ziyou_maxcapac = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			ziyou_maxcapac = ziyou_maxcapac*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 5 &&  control_id == 7)						/*取得动态电容的最小值设定*/
	{
		dongtai_mincapac = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			dongtai_mincapac = dongtai_mincapac*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 5 &&  control_id == 16)						/*取得动态电容的最大值设定*/
	{
		dongtai_maxcapac = 0;
    	while(((uint8 *)(&msg->param))[i])
		{
			dongtai_maxcapac = dongtai_maxcapac*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
		else if(screen_id == 5 &&  control_id == 8)						/*取得动态电感的最小值设定*/
	{
		dongtai_minprod = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			dongtai_minprod = dongtai_minprod*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 5 &&  control_id == 17)						/*取得动态电感的最大值设定*/
	{
		dongtai_maxprod = 0;
    	while(((uint8 *)(&msg->param))[i])
		{
			dongtai_maxprod = dongtai_maxprod*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 5 &&  control_id == 9)						/*取得反谐振阻抗的最小值设定*/
	{
		fanxiezhen_minzukang = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			fanxiezhen_minzukang = fanxiezhen_minzukang*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 5 &&  control_id == 18)						/*取得反谐振阻抗的最大值设定*/
	{
		fanxiezhen_maxzukang = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			fanxiezhen_maxzukang = fanxiezhen_maxzukang*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 5 &&  control_id == 10)						/*取得品质因数的最小值设定*/
	{
		pinzhiyinshu_min = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			pinzhiyinshu_min = pinzhiyinshu_min*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	else if(screen_id == 5 &&  control_id == 19)						/*取得品质因数的最大值设定*/
	{
		pinzhiyinshu_max = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			pinzhiyinshu_max = pinzhiyinshu_max*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		}
	}
	
}


/**************************************************************************************************************
* 名    称： OnRecmenu
* 功    能： 菜单消息响应函数
* 入口参数： msg-指令缓冲区，size-指令大小
***************************************************************************************************************/
void OnRecmenu(PCTRL_MSG msg,qsize size)
{
// 	uint32 value = PTR2U32(msg->param);     //数值
 	screen_id = PTR2U16(&msg->screen_id);   //画面ID
 	control_id = PTR2U16(&msg->control_id); //控件ID
    if(screen_id == 0 &&  control_id == 25)        /*获取系统自带键盘输入 */
  {
		if(Ok_Control_Flag == 1)
		{
			StarFre_Control_Flag = 1;
		}
		else
		{
			StarFre_Control_Flag = 0;
		}
		number_menu = 0;
 		number_menu =(((uint8 *)(&msg->param))[1] - 0x30);  /*从接收缓冲区取出键盘输入的数字，再转换成十进制数字*/
		switch(number_menu)
			{
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
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
