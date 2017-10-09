/*********************************************************************
*                Copyright (C), 2015-2016, Supersonics. Co., Ltd.
*                        �迹��������������
*
*                          Ӳ��ƽ̨: xxx
*                          �� о Ƭ: STM32F103
*                          �� Ŀ ��: xxx
**********************************************************************
*�ļ���: CtrFile.c
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

#include "CtrFile.h"

uint16 screen_id;                        /*ҳ��ID��ű���   */
uint16 control_id;                       /*�ؼ�ID��ű���   */
volatile uint16 current_screen_id;       /*��ǰ��Ļ���ڵĻ����ű���   */

/************************������ؼ���־λ*****************************/
uint16 Ok_Control_Flag = 0;
uint16 Stop_Control_Flag = 0;
uint16 Clear_Flag = 0;
uint16 SaveData_Flag = 0;
uint16 SavePic_Flag = 0;
uint16 StarFre_Control_Flag = 0;
uint16 EndFre_Control_Flag = 0;
uint16 Dac_Control_Flag = 0;
uint16 Ok1_Control_Flag = 0;     //DGUT

u32 start_fre = 0;                       /*������ʼƵ��   */
u32 end_fre = 0;                         /*������ֹƵ��   */


/*****Һ�����������*****/
u32 min_freq = 0;                       /*������ʼƵ��   */
u32 max_fre = 0;                         /*������ֹƵ��   */
u32 min_resis = 0;                       /*������С����   */
u32 max_resis = 0;                         /*����������   */
u32 min_capac = 0;                       /*������С����*/
u32 max_capac = 0;                         /*����������   */
u32 min_induc = 0;                       /*������С���   */
u32 max_indut = 0;                         /*���������   */


u32 dac_value = 0;                       /*����DAC��ֵ    */
u32 file_name=0;         //DGUT


/*********************************************************************
* �������ã���ʼ��Һ����UI
* ������������
* ��������ֵ����
**********************************************************************/
void iniuserctr(void)
{
	int i = 9;
  /*�޸��ı�����ʾ����*/
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
* ��    �ƣ� OnRecvChangeScreen
* ��    �ܣ� �����л���Ϣ��Ӧ����
* ��ڲ����� screen_id-�л���Ļ���ID
***************************************************************************************************************/
void OnRecvChangeScreen(uint16_t screen_id)
{
}

/**************************************************************************************************************
* ��    �ƣ� OnRecvTouchXY
* ��    �ܣ� �������������Ӧ
* ��ڲ����� press-1���´�������3�ɿ�������
* ��ڲ����� x-����X����
* ��ڲ����� y-����y����
***************************************************************************************************************/
void OnRecvTouchXY(uint8_t press,uint16_t x,uint16_t y)
{
    //TODO: �û�����
}

/**************************************************************************************************************
* ��    �ƣ� OnRecvButton
* ��    �ܣ� ��ť��Ϣ��Ӧ����
* ��ڲ����� msg-ָ�������size-ָ���С
***************************************************************************************************************/
void OnRecvButton(PCTRL_MSG msg,qsize size)
{
	int i = 0;
  screen_id = PTR2U16(&msg->screen_id);     /*��ȡ����ID   */
  control_id = PTR2U16(&msg->control_id);   /*��ȡ�ؼ�ID   */

  /**************************/
  /*-- SCREEN 0�Ļ��洦�� --*/
  /**************************/
  if(screen_id == 0 && control_id == 3)     /*"ȷ��"�İ�����������   */
  {
		Ok_Control_Flag = 1;
		AnimationPlayFrame(0,2,1);
		GraphChannelDataClear(0,23,0);
		GraphChannelDataClear(0,33,0);

		Delayus(100000);
		GraphChannelAdd(0,23,0,BLUE);           /*�����λ����ͨ��   */
		GraphChannelAdd(0,33,0,RED);            /*����迹����ͨ��   */

		GetControlValue(0,6);				            /*��ȡ��ʼƵ��   */
		GetControlValue(0,7);				            /*��ȡ��ֹƵ��   */
		GetControlValue(0,8);				            /*��ȡDAC����ֵ	 */	
  }
	else if(screen_id == 0 && control_id == 4)	/*"ֹͣ"�İ���������   */
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
* ��    �ƣ� OnRecvText
* ��    �ܣ� �ı��ؼ���Ϣ��Ӧ����
* ��ڲ����� msg-ָ�������size-ָ���С
***************************************************************************************************************/
void OnRecvText(PCTRL_MSG msg, qsize size)
{
	uint32 i = 0;
  screen_id = PTR2U16(&msg->screen_id);         /*��ȡ����ID */
  control_id = PTR2U16(&msg->control_id);       /*��ȡ�ؼ�ID */

  if(screen_id == 0 &&  control_id == 6)        /*��ȡϵͳ�Դ��������� */
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
			start_fre = start_fre*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
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
			end_fre = end_fre*10+ (((uint8 *)(&msg->param))[i++] - 0x30);      /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ�������� */
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
			dac_value = dac_value*10+ (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ�������� */
		}
	}//****DGUT
	
	else if(screen_id == 1 &&  control_id == 2)
	{
		file_name = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			file_name= file_name*10+ (((uint8 *)(&msg->param))[i++] -0x30);      /*�ӽ��ջ�����ȡ��������������֣��ļ���������ת����ʮ�������� */		}
	}//**DGUT

	
	else if(screen_id == 6 &&  control_id == 3)				/*ȡ��Ƶ�ʵ���Сֵ�趨*/
	{
		min_freq = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			min_freq = min_freq*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 6 &&  control_id == 4)				/*ȡ��Ƶ�ʵ����ֵ�趨*/
	{
		
		max_fre = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			max_fre = max_fre*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	
	}


	else if(screen_id == 6 &&  control_id == 5)					/*ȡ�õ������Сֵ�趨*/
	{
		
		min_resis = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			min_resis = min_resis*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 6 &&  control_id == 6)							/*ȡ�õ�������ֵ�趨*/
	{
		
		max_resis = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			max_resis = max_resis*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}



	else if(screen_id == 6 &&  control_id == 7)						/*ȡ�õ��ݵ���Сֵ�趨*/
	{
		min_capac = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			min_capac = min_capac*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 6 &&  control_id == 8)						/*ȡ�õ��ݵ����ֵ�趨*/
	{
		
		max_capac = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			max_capac = max_capac*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
		else if(screen_id == 6 &&  control_id == 8)						/*ȡ�õ�е���Сֵ�趨*/
	{
		
		min_induc = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			min_induc = min_induc*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
			else if(screen_id == 6 &&  control_id == 8)						/*ȡ�õ�е����ֵ�趨*/
	{
		max_indut = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			max_indut = max_indut*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
}

/**************************************************************************************************************
* ��    �ƣ� OnRecvProgress
* ��    �ܣ� ��������Ϣ��Ӧ����
* ��ڲ����� msg-ָ�������size-ָ���С
***************************************************************************************************************/
void OnRecvProgress(PCTRL_MSG msg,qsize size)
{
// 	uint32 value = PTR2U32(msg->param);     //��ֵ
// 	screen_id = PTR2U16(&msg->screen_id);   //����ID
// 	control_id = PTR2U16(&msg->control_id); //�ؼ�ID
    //TODO: �û�����
}

/**************************************************************************************************************
* ��    �ƣ� OnRecvSlider
* ��    �ܣ� ��������Ϣ��Ӧ����
* ��ڲ����� msg-ָ�������size-ָ���С
***************************************************************************************************************/
void OnRecvSlider(PCTRL_MSG msg,qsize size)
{
	;
}

/**************************************************************************************************************
* ��    �ƣ� OnRecvMeter
* ��    �ܣ� �Ǳ���Ϣ��Ӧ����
* ��ڲ����� msg-ָ�������size-ָ���С
***************************************************************************************************************/
void OnRecvMeter(PCTRL_MSG msg,qsize size)
{
	;
}

/**************************************************************************************************************
* ��    �ƣ� OnRecvMeter
* ��    �ܣ� ͼ��ؼ���Ӧ����
* ��ڲ����� msg-ָ�������size-ָ���С
***************************************************************************************************************/
void OnRecvAnimation(PCTRL_MSG msg,qsize size)
{
//     uint16 screen_id = PTR2U16(&msg->screen_id);  //����ID
//     uint16 control_id = PTR2U16(&msg->control_id);//�ؼ�ID
//     uint32 value = PTR2U32(msg->param);           
	
    //TODO: �û�����
}

/**************************************************************************************************************
* ��    �ƣ� OnRecCurrentScreen
* ��    �ܣ� ����״̬��Ӧ����
* ��ڲ����� msg-ָ�������size-ָ���С
***************************************************************************************************************/
void	OnRecCurrentScreen(PCTRL_MSG msg,qsize size)
{
	 current_screen_id = PTR2U16(&msg->screen_id);
}

/*****************************�����ǰ�ť�ؼ�����Ϣ������*********************************/
