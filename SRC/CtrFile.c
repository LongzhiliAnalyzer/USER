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
uint16 SaveData_Ok_Flag = 0;     //DGUT
uint16 Display_Mode_Flag = 0;     //��ʾģʽ��־λ

u32 start_fre = 0;                       /*������ʼƵ��   */
u32 end_fre = 0;                         /*������ֹƵ��   */


/*****Һ�����������*****/
u32 xiezhen_minfreq = 0;                       /*����г����СƵ��   */
u32 xiezhen_maxfreq = 0xFFFFFFFF;                         /*����г�����Ƶ��   */
u32 fanxiezhen_minfreq = 0;                       /*���巴г����СƵ��   */
u32 fanxiezhen_maxfreq = 0xFFFFFFFF;                         /*���巴г�����Ƶ��   */

u32 dongtai_minresis = 0;                       /*���嶯̬��С����   */
u32 dongtai_maxresis = 0xFFFFFFFF;                         /*���嶯̬������   */

u32 jingtai_mincapac = 0;                       /*���徲̬��С����*/
u32 jingtai_maxcapac = 0xFFFFFFFF;                         /*���徲̬������   */
u32 ziyou_mincapac = 0;                       /*����������С����*/
u32 ziyou_maxcapac = 0xFFFFFFFF;                         /*��������������   */
u32 dongtai_mincapac = 0;                       /*���嶯̬��С����*/
u32 dongtai_maxcapac = 0xFFFFFFFF;                         /*���嶯̬������   */
u32 dongtai_minprod = 0;                       /*���嶯̬��С���*/
u32 dongtai_maxprod = 0xFFFFFFFF;                         /*���嶯̬�����   */


u32 fanxiezhen_minzukang = 0;                       /*������С��г���迹   */
u32 fanxiezhen_maxzukang = 0xFFFFFFFF;                         /*�������г���迹*/
u32 pinzhiyinshu_min = 0;                       /*������СƷ������   */
u32 pinzhiyinshu_max = 0xFFFFFFFF;                         /*�������Ʒ������*/

u32 dac_value = 0;                       /*����DAC��ֵ    */
u32 file_name=0;         //DGUT
u32 number_menu = 0;                       /*������ʾ�˵�ѡ�����*/


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
	else if(screen_id == 0 && control_id == 29)//�����ʣ�����
	{
		SavePic_Flag = 1;
	}
	//DGUT
	else if(screen_id == 1 && control_id == 3)
	{
		SaveData_Ok_Flag = 1;
	}

	//����������ʾģʽ

	
	else if(screen_id == 5 && control_id == 21)
	{
			
		 xiezhen_minfreq = 0;	 SetTextValue( 5, 2,"0");				   /*����г����СƵ��	*/
		 xiezhen_maxfreq = 0xFFFFFFFF;	 SetTextValue( 5, 11,"999999");					  /*����г�����Ƶ��   */
		 fanxiezhen_minfreq = 0;  SetTextValue( 5, 3,"0");					  /*���巴г����СƵ��   */
		 fanxiezhen_maxfreq = 0xFFFFFFFF; SetTextValue( 5, 12,"999999");						 /*���巴г�����Ƶ��	 */
		 dongtai_minresis = 0;	 SetTextValue( 5, 4,"0");	/*���嶯̬��С����	 */
		 dongtai_maxresis = 0xFFFFFFFF;	 SetTextValue( 5, 13,"999999");   /*���嶯̬������	*/
		
		 jingtai_mincapac = 0;	 SetTextValue( 5, 5,"0");/*���徲̬��С����*/
		 jingtai_maxcapac = 0xFFFFFFFF; SetTextValue( 5, 14,"999999");		   /*���徲̬������	*/
		 ziyou_mincapac = 0; 	 SetTextValue( 5, 6,"0");	  /*����������С����*/
		 ziyou_maxcapac = 0xFFFFFFFF; SetTextValue( 5, 15,"999999");		 /*��������������   */
		 dongtai_mincapac = 0;	 SetTextValue( 5, 7,"0");/*���嶯̬��С����*/
		 dongtai_maxcapac = 0xFFFFFFFF; SetTextValue( 5, 16,"999999");	   /*���嶯̬������	*/
		 dongtai_minprod = 0;	 SetTextValue( 5, 8,"0");   /*���嶯̬��С���*/
		 dongtai_maxprod = 0xFFFFFFFF;	 SetTextValue( 5, 17,"999999");		  /*���嶯̬�����   */
		
		 fanxiezhen_minzukang = 0;	 SetTextValue( 5, 9,"0");	/*������С��г���迹	*/
		 fanxiezhen_maxzukang = 0xFFFFFFFF; SetTextValue( 5, 18,"999999");	   /*�������г���迹*/
		 pinzhiyinshu_min = 0;	 SetTextValue( 5, 10,"0");	/*������СƷ������	 */
		 pinzhiyinshu_max = 0xFFFFFFFF;	 SetTextValue( 5, 19,"999999");		   /*�������Ʒ������*/
	}
	else if(screen_id == 14 && control_id == 2)//������ʾģʽ   1��Ƶ��__�迹log  2��
	{
		 Display_Mode_Flag=0;
		 chart( Display_Mode_Flag);
	}
	else if(screen_id == 14 && control_id == 3)//������ʾģʽ   1��Ƶ�ʡ�������     2��
	{
		Display_Mode_Flag=1;
		chart( Display_Mode_Flag);
	}
	else if(screen_id == 14 && control_id == 4)//������ʾģʽ
	{
		Display_Mode_Flag=2;
		chart( Display_Mode_Flag);
	}
	else if(screen_id == 14 && control_id == 5)//������ʾģʽ
	{
		Display_Mode_Flag=3;
		chart( Display_Mode_Flag);
	}
	else if(screen_id == 14 && control_id == 6)//������ʾģʽ
	{
		Display_Mode_Flag=4;
		chart( Display_Mode_Flag);
	}
	else if(screen_id == 14 && control_id == 7)//������ʾģʽ
	{
		Display_Mode_Flag=5;
		chart( Display_Mode_Flag);
	}
	
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

	
	else if(screen_id == 5 &&  control_id == 2)				/*ȡ��г��Ƶ�ʵ���Сֵ�趨*/
	{
		xiezhen_minfreq = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			xiezhen_minfreq = xiezhen_minfreq*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 5 &&  control_id == 11)				/*ȡ��г��Ƶ�ʵ����ֵ�趨*/
	{
		
		xiezhen_maxfreq = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			xiezhen_maxfreq = xiezhen_maxfreq*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	
	}


	else if(screen_id == 5 &&  control_id == 3)					/*ȡ�÷�г��Ƶ�ʵ���Сֵ�趨*/
	{
		
		fanxiezhen_minfreq = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			fanxiezhen_minfreq = fanxiezhen_minfreq*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 5 &&  control_id == 12)							/*ȡ�÷�г��Ƶ�ʵ����ֵ�趨*/
	{
		
		fanxiezhen_maxfreq = 0;
    while(((uint8 *)(&msg->param))[i])
		{
			fanxiezhen_maxfreq = fanxiezhen_maxfreq*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}



	else if(screen_id == 5 &&  control_id == 4)						/*ȡ�ö�̬�������Сֵ�趨*/
	{
		dongtai_minresis = 0;
    	while(((uint8 *)(&msg->param))[i])
		{
			dongtai_minresis = dongtai_minresis*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 5 &&  control_id == 13)						/*ȡ�ö�̬��������ֵ�趨*/
	{
		
		dongtai_maxresis = 0;
    	while(((uint8 *)(&msg->param))[i])
		{
			dongtai_maxresis = dongtai_maxresis*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 5 &&  control_id == 5)						/*ȡ�þ�̬���ݵ���Сֵ�趨*/
	{
		
		jingtai_mincapac = 0;
    	while(((uint8 *)(&msg->param))[i])
		{
			jingtai_mincapac = jingtai_mincapac*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 5 &&  control_id == 14)						/*ȡ�þ�̬���ݵ����ֵ�趨*/
	{
		jingtai_maxcapac = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			jingtai_maxcapac = jingtai_maxcapac*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 5 &&  control_id == 6)						/*ȡ�����ɵ��ݵ���Сֵ�趨*/
	{
		ziyou_mincapac = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			ziyou_mincapac = ziyou_mincapac*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 5 &&  control_id == 15)						/*ȡ�����ɵ��ݵ����ֵ�趨*/
	{
		ziyou_maxcapac = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			ziyou_maxcapac = ziyou_maxcapac*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 5 &&  control_id == 7)						/*ȡ�ö�̬���ݵ���Сֵ�趨*/
	{
		dongtai_mincapac = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			dongtai_mincapac = dongtai_mincapac*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 5 &&  control_id == 16)						/*ȡ�ö�̬���ݵ����ֵ�趨*/
	{
		dongtai_maxcapac = 0;
    	while(((uint8 *)(&msg->param))[i])
		{
			dongtai_maxcapac = dongtai_maxcapac*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
		else if(screen_id == 5 &&  control_id == 8)						/*ȡ�ö�̬��е���Сֵ�趨*/
	{
		dongtai_minprod = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			dongtai_minprod = dongtai_minprod*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 5 &&  control_id == 17)						/*ȡ�ö�̬��е����ֵ�趨*/
	{
		dongtai_maxprod = 0;
    	while(((uint8 *)(&msg->param))[i])
		{
			dongtai_maxprod = dongtai_maxprod*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 5 &&  control_id == 9)						/*ȡ�÷�г���迹����Сֵ�趨*/
	{
		fanxiezhen_minzukang = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			fanxiezhen_minzukang = fanxiezhen_minzukang*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 5 &&  control_id == 18)						/*ȡ�÷�г���迹�����ֵ�趨*/
	{
		fanxiezhen_maxzukang = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			fanxiezhen_maxzukang = fanxiezhen_maxzukang*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 5 &&  control_id == 10)						/*ȡ��Ʒ����������Сֵ�趨*/
	{
		pinzhiyinshu_min = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			pinzhiyinshu_min = pinzhiyinshu_min*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	else if(screen_id == 5 &&  control_id == 19)						/*ȡ��Ʒ�����������ֵ�趨*/
	{
		pinzhiyinshu_max = 0;
   		 while(((uint8 *)(&msg->param))[i])
		{
			pinzhiyinshu_max = pinzhiyinshu_max*10 + (((uint8 *)(&msg->param))[i++] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
		}
	}
	
}


/**************************************************************************************************************
* ��    �ƣ� OnRecmenu
* ��    �ܣ� �˵���Ϣ��Ӧ����
* ��ڲ����� msg-ָ�������size-ָ���С
***************************************************************************************************************/
void OnRecmenu(PCTRL_MSG msg,qsize size)
{
// 	uint32 value = PTR2U32(msg->param);     //��ֵ
 	screen_id = PTR2U16(&msg->screen_id);   //����ID
 	control_id = PTR2U16(&msg->control_id); //�ؼ�ID
    if(screen_id == 0 &&  control_id == 25)        /*��ȡϵͳ�Դ��������� */
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
 		number_menu =(((uint8 *)(&msg->param))[1] - 0x30);  /*�ӽ��ջ�����ȡ��������������֣���ת����ʮ��������*/
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
