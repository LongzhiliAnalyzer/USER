/*********************************************************************
*                Copyright (C), 2015-2016, Supersonics. Co., Ltd.
*                        ×è¿¹·ÖÎöÒÇÇý¶¯³ÌÐò
*
*                          Ó²¼þÆ½Ì¨: xxx
*                          Ö÷ Ð¾ Æ¬: STM32F103
*                          Ïî Ä¿ ×é: xxx
**********************************************************************
*ÎÄ¼þÃû: control.c
*°æ  ±¾: V1.0.0
*×÷  Õß: 
*ÈÕ  ÆÚ: 
*Ëµ  Ã÷:
**********************************************************************
*ÖØÒª¹±Ï×Õß: 
**********************************************************************
*Àú      Ê·: 
*1. <ÐÞ¸ÄÕß>     <ÐÞ¸ÄÈÕÆÚ>     <ÐÞ¸ÄËµÃ÷>

*********************************************************************/

#include "control.h"
#include "CtrFile.h"

#include "cmd_queue.h"
#include "AD9833.h"
#include <math.h>
#include <stdio.h>

#include <stdarg.h>

u8 USART2_RX=0;

u32 Current_Fre=0;	//µ±Ç°ÆµÂÊ
u16 Current_V = 0;	//µ±Ç°²É¼¯µÄµçÑ¹Öµ
u16 Current_A = 0;	//µ±Ç°²É¼¯µÄµçÁ÷Öµ
u32 Impandence_Value = 0;//×è¿¹Öµ

u32 Impandence_Value_Buffer=1500;

u32 FreGain = 5.0;                               /*É¨Æµ²½½øµÄ±äÁ¿*/

u16 ImpandenceStatus = Res01;                    /*×è¿¹ÇÐ»»Ê±£¬½øÐÐ×´Ì¬Ñ¡Ôñ*/
float SampleRes=5.0;                             /*×è¿¹±ê¼Ç*/
u32 Mid_Fre = 0;			                           /*¶¨Òå²¢³õÊ¼»¯Ð³ÕñÆµÂÊµã*/
float Current_ARes =0.0;                         /*µçÁ÷±ê¼Ç*/

s32 angle_P = 0;

u32 Impandence_Buffer[2048] = {0};               /*¶¨ÒåÒ»¸ö´óµÄÊý×é£¬ÓÃÓÚ±£´æÉ¨Æµ¹ý³ÌÖÐµÃµ½µÄ×è¿¹Öµ*/
u16 Impandence_Buffer2[2048] = {0};              /*¶¨ÒåÒ»¸ö´óµÄÊý×é£¬ÓÃÓÚ±£´æ¶ÔÊý×ª»»ºóµÄ×è¿¹Öµ*/
u16 Impandence_Log10[2048] = {0};
s16 Angle_Buffer[2048] = {0};                    /*¶¨ÒåÒ»¸ö´óµÄÊý×é£¬ÓÃÓÚ±£´æÉ¨Æµ¹ý³ÌÖÐµÃµ½µÄÏàÎ»Öµ£¨Î´¾­×ª»»£©*/
float Angle[2048] = {0.0};                       /*¶¨ÒåÒ»¸ö´óµÄÊý×é£¬ÓÃÓÚ±£´æÉ¨Æµ¹ý³ÌÖÐµÃµ½µÄÏàÎ»Öµ*/
u32 Fre_Buffer[2048] = {0};                      /*±£´æÉ¨Æµ¹ý³ÌÖÐµÄÆµÂÊµãÊý*/
u16 Impandence_Buffer_Flag = 0;                  /*ÓÃÓÚ¼ÇÂ¼É¨ÆµµÄÎ»ÖÃ*/
u32 Impandence_Value_Max = 0;                    /*×î´ó×è¿¹Öµ*/
u32 Impandence_Value_Min = 0;                    /*×îÐ¡×è¿¹Öµ*/
u32 Impandence_G[2048] = {0};                    /*   */
u32 Impandence_G_Buffer[2048] = {0};             /*   */
u32 XJ_Temp = 0;                                 /*   */
u32 XZ_Impandence = 0;                           /*   */
u32 YJ_Temp = 0;                                 /*   */
u32 YZ_Impandence = 0;                           /*   */
u16 Impandence_Log10_Max = 0;                    /*¶ÔÊý×ª»»ÒÔºóµÄ×è¿¹×î´óÖµ*/
u16 Impandence_Log10_Min = 0;                    /*¶ÔÊý×ª»»ÒÔºóµÄ×è¿¹×îÐ¡Öµ*/
u32 Fre_Max = 0;                                 /*·´Ð³Õñ×è¿¹*/
u32 Fre_Min = 0;                                 /*Ð³Õñ×è¿¹*/
s16 Angle_Buffer0 = 0;

u16 MaxValue_Flag = 0;
u16 MinValue_Flag = 0;


double kkk = 0.0;
double CT = 0.0;                                 /*ÓÃÓÚ¼ÆËã¾²Ì¬µçÈÝC0µÄ×ª»»Òò×Ó*/

double Qm = 0.0;                                 /*¶¨Òå²¢³õÊ¼»¯»úÐµÆ·ÖÊÒòËØQm*/
double L1 = 0.0;                                 /*¶¨Òå²¢³õÊ¼»¯¶¯Ì¬µç¸ÐL1*/
double C1 = 0.0;                                 /*¶¨Òå²¢³õÊ¼»¯¶¯Ì¬µçÈÝC1*/
double C0 = 0.0;                                 /*¶¨Òå²¢³õÊ¼»¯¾²Ì¬µçÈÝC1*/
double Keff = 0.0;                               /*¶¨Òå²¢³õÊ¼»¯»úµçñîºÏÏµÊý*/
u32 Fre_F1 = 0;			       //¶¨Òå²¢³õÊ¼»¯°ë¹¦ÂÊµãF1
u32 Fre_F2 = 0;				     //¶¨Òå²¢³õÊ¼»¯°ë¹¦ÂÊµãF2
u32 fd = 0;

//u32 file_name=0;         //DGUT
/******************************************************************
* º¯Êý×÷ÓÃ£º¶ÔÊý¾Ý½øÐÐÖÐÖµÂË²¨
* º¯Êý²ÎÊý£º1¡¢Êý¾ÝÖ¸Õë£»2¡¢ÂË²¨´ÎÊý
* º¯Êý·µ»ØÖµ£ºÂË²¨ºóµÄÖÐ¼äÊýÖµ
******************************************************************/
u16 MidFilter(u16* Array,u8 num )
{
	u16 temp=0;
	u16 mid=0;
	u16 i=0,j=0;
	for(i=0;i<num-1;i++)
	{
		for(j=i+1;j<num;j++)
		{
			if(Array[i]>Array[j])
			{
				temp=Array[i];
				Array[i]=Array[j];
				Array[j]=temp;
			}
		}
	}
	if(num%2 != 0)
	{
		mid=Array[num/2];
	}
	else
	{
		mid=(Array[num/2-1]+Array[num/2])/2;
	}
	return mid;
}
/******************************************************************
* º¯Êý×÷ÓÃ£º¶ÔÊý¾Ý½øÐÐ¾ùÖµÂË²¨
* º¯Êý²ÎÊý£º1¡¢Êý¾ÝÖ¸Õë£»2¡¢ÂË²¨´ÎÊý
* º¯Êý·µ»ØÖµ£ºÂË²¨ºóµÄÆ½¾ùÊýÖµ
******************************************************************/
u16 AverageFilter(u16* Array,u8 num)
{
	u32 average=0;
	u16 i = 0;
	for(i=0;i<num;i++)
	{
		average+=Array[i];
	}
	average/=num;
	return ((u16)average);
}

/******************************************************************
* º¯Êý×÷ÓÃ£º¶ÔÊý¾Ý½øÐÐÖÐÖµÆ½¾ùÂË²¨£¬¶Ônum¸ö²É¼¯Êý¾Ý£¬È¥µô×î´ó×îÐ¡Öµ£¬ÔÙÇóËãÊõÆ½¾ùÖµ
* º¯Êý²ÎÊý£º1¡¢Êý¾ÝÖ¸Õë£»2¡¢ÂË²¨´ÎÊý
* º¯Êý·µ»ØÖµ£ºÂË²¨ºóµÄÆ½¾ùÊýÖµ
******************************************************************/
u16 MidAveFilter(u16* Array,u8 num)
{
	u16 min,max,sum=0;
	u16 i = 0,j=0,k=0;;
	min=Array[0];
	for(i=1;i<num;i++)
	{
		if(min>Array[i])
		{
			min=Array[i];
		}
	}
	max=Array[0];
	for(j=1;j<num;j++)
	{
		if(max<Array[j])
		{
			max=Array[j];
		}
	}
	for(k=0;k<num;k++)
	{
		sum+=Array[k];
	}
	return (sum-max-min)/(num-2);
}
/****************************************************************/
s16 MidFilterSigned(s16* Array,u16 num )
{
	s16 temp=0;
	s16 mid=0;
	u16 i=0,j=0;
	for(i=0;i<num-1;i++)
	{
		for(j=i+1;j<num;j++)
		{
			if(Array[i]>Array[j])
			{
				temp=Array[i];
				Array[i]=Array[j];
				Array[j]=temp;
			}
		}
	}
	if(num%2 != 0)
	{
		mid=Array[num/2];
	}
	else
	{
		mid=(Array[num/2-1]+Array[num/2])/2;
	}
	return mid;
}
/******************************************************************
º¯ÊýÃû³Æ£ºvoid Phase_ValueFilter(u8 num)
¹¦    ÄÜ£ºÂË²¨´¦Àíº¯Êý,¶ÔÏàÎ»½øÐÐÂË²¨´¦Àí
Ëµ    Ã÷£º
Èë¿Ú²ÎÊý£ºÂË²¨½×Êýý?um(0¡¢1¡¢2¡¢3...20)
·µ»ØÖµ  £ºÎÞ
********************************************************************/
void Phase_ValueFilter(u8 num)
{
	u16 i = 0;
	s16 Phase[30] = {0};

	TIM4->CCER &= (uint16_t)~((uint16_t)TIM_CCER_CC2E);	         //½ûÖ¹¶þÍ¨µÀ²¶»ñ
	TIM4->CCER |= (uint16_t)TIM_CCER_CC1E;				               //ÔÊÐíÒ»Í¨µÀ²¶»ñ

	for(i=0;i<num;i++)
	{
		CaptureNumber1 = 0;

		if(CaptureNumber1 != 3)
			Delayus(1000);

		Phase[i]=angle;
	}
	angle_P = MidFilterSigned(Phase,num);
}


/******************************************************************
º¯ÊýÃû³Æ£ºvoid ADC1_ValueFilter(u8 num) ¹¦ÄÜÊÇµÃµ½²ÉÑùµÄµçÑ¹¡¢µçÁ÷Öµ
¹¦    ÄÜ£ºÂË²¨´¦Àíº¯Êý //²ÉÑù²¢¾­¹ýÖÐÖµÂË²¨¶øµÃµ½µÄµçÑ¹ºÍµçÁ÷Öµ£»
Ëµ    Ã÷£º
Èë¿Ú²ÎÊý£ºÂË²¨½×Êýý?um(0¡¢1¡¢2¡¢3...20)
·µ»ØÖµ  £ºÎÞ
********************************************************************/
void ADC1_ValueFilter(u8 num)
{
	u16 TempVoltData[20] = {0};
	u16 TempAmpData[20] = {0};
	u16 i=0;

 	for(i=0;i<num;i++)
	{
		TempVoltData[i]=ADCConvertedValue[0];
		Delayus(100);
		TempAmpData[i]=ADCConvertedValue[1];

		Delayus(100);
	}

	Current_V=MidFilter(TempVoltData,num);
	Current_A=MidFilter(TempAmpData,num);
}

/**********************************************************************
º¯ÊýÃû³Æ: void Send_Cmd(void)
¹¦    ÄÜ: ·¢ËÍCTµçÈÝÖµµÄÊý¾ÝÖ¸Áî
Ëµ    Ã÷:
Èë¿Ú²ÎÊý:
·µ »Ø Öµ: ÎÞ
***********************************************************************/
void Send_Cmd(void)
{
	u8 Parity = 0;
	u32 Capacitor = 0;
	Capacitor = CT*1000000;

	USART_SendData(USART1, 0x3C);  //·¢ËÍÍ¬²½×Ö·û
	USART_SendData(USART1, 0xA2);

	USART_SendData(USART1, 0x16);
  Parity ^= 0x16;
	USART_SendData(USART1, 0x02);
  Parity ^= 0x02;

	USART_SendData(USART1,  (unsigned char)Capacitor);
  Parity ^= (unsigned char)Capacitor;
	USART_SendData(USART1,  (unsigned char)(Capacitor>>8));
  Parity ^= (unsigned char)(Capacitor>>8);
	USART_SendData(USART1,  (unsigned char)(Capacitor>>16));
  Parity ^= (unsigned char)(Capacitor>>16);
	USART_SendData(USART1,  (unsigned char)(Capacitor>>24));
  Parity ^= (unsigned char)(Capacitor>>24);

	USART_SendData(USART1, (unsigned char)0);
  Parity ^= (unsigned char)0;
	USART_SendData(USART1, (unsigned char)(0));
  Parity ^= (unsigned char)(0);

	USART_SendData(USART1,  (unsigned char)0);
  Parity ^= (unsigned char)0;
	USART_SendData(USART1,  (unsigned char)(0));
  Parity ^= (unsigned char)(0);
	USART_SendData(USART1,  (unsigned char)(0));
	Parity ^= (unsigned char)(0);

	USART_SendData(USART1, (unsigned char)0);
	Parity ^= (unsigned char)0;
	USART_SendData(USART1, (unsigned char)(0));
	Parity ^= (unsigned char)(0);

	USART_SendData(USART1, (unsigned char)0);
	Parity ^= (unsigned char)0;
	USART_SendData(USART1, (unsigned char)(0));
	Parity ^= (unsigned char)0;

	USART_SendData(USART1, (unsigned char)0);
	Parity ^= (unsigned char)0;
	USART_SendData(USART1, (unsigned char)(0));
	Parity ^= (unsigned char)(0);

	USART_SendData(USART1, (unsigned char)0);
	Parity ^= (unsigned char)0;
	USART_SendData(USART1, (unsigned char)(0));
	Parity ^= (unsigned char)0;

	USART_SendData(USART1, (unsigned char)0);
	Parity ^= (unsigned char)0;
	USART_SendData(USART1, (unsigned char)(0));
	Parity ^= (unsigned char)(0);

	USART_SendData(USART1, Parity);
	Parity = 0;
	Delayus(8000);
}

/*********************************************************************
* º¯Êý×÷ÓÃ£º×è¿¹ÇÐ»»ÅÐ¶Ïº¯Êý
* º¯Êý²ÎÊý£ºÎÞ
* º¯Êý·µ»ØÖµ£ºÎÞ
*********************************************************************/
void Impandence_Switch(void)
{
	switch(ImpandenceStatus)
	{
		case Res01:
		{
			if(Current_A<=40)
			{
				ImpandenceStatus = Res02;
				GPIO_ResetBits(GPIOE,GPIO_Pin_14);
				GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15);
				Delayus(500000);
				SampleRes = 10.0;
			}
		}break;
		case Res02:
		{
			if(Current_A<=40)
			{
				ImpandenceStatus = Res03;
				GPIO_ResetBits(GPIOE,GPIO_Pin_13);
        GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_14|GPIO_Pin_15);
				Delayus(500000);
				SampleRes = 30.0;
			}
			else if(Current_A>=200)
			{
				ImpandenceStatus = Res01;
				GPIO_ResetBits(GPIOE,GPIO_Pin_15);
        GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);
				Delayus(500000);
				SampleRes = 5.0;
			}
		}break;
		case Res03:
		{
			if(Current_A<=40)
			{
				ImpandenceStatus = Res04;
				GPIO_ResetBits(GPIOE,GPIO_Pin_12);
        GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
				Delayus(500000);
				SampleRes = 100.0;
			}
			else if(Current_A>=200)
			{
				ImpandenceStatus = Res02;
				GPIO_ResetBits(GPIOE,GPIO_Pin_14);
        GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15);
				Delayus(500000);
				SampleRes = 10.0;
			}
		}break;
		case Res04:
		{
			if(Current_A<=40)
			{
				ImpandenceStatus = Res05;
				GPIO_ResetBits(GPIOE,GPIO_Pin_11);
        GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
				Delayus(500000);
				SampleRes = 300.0;
			}
			else if(Current_A>=250)
			{
				ImpandenceStatus = Res03;
				GPIO_ResetBits(GPIOE,GPIO_Pin_13);
        GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_14|GPIO_Pin_15);
				Delayus(500000);
				SampleRes = 30.0;
			}
		}break;
		case Res05:
		{
			if(Current_A<=40)
			{
				ImpandenceStatus = Res06;
				GPIO_ResetBits(GPIOE,GPIO_Pin_10);
        GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
				Delayus(500000);
				SampleRes = 1000.0;
			}
			else if(Current_A>=250)
			{
				ImpandenceStatus = Res04;
				GPIO_ResetBits(GPIOE,GPIO_Pin_12);
        GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
				Delayus(500000);
				SampleRes = 100.0;
			}
		}break;
		case Res06:
		{
			if(Current_A<=40)
			{
				ImpandenceStatus = Res07;
				GPIO_ResetBits(GPIOE,GPIO_Pin_9);
        GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
				Delayus(500000);
				SampleRes = 3000.0;
			}
			else if(Current_A>=250)
			{
				ImpandenceStatus = Res05;
				GPIO_ResetBits(GPIOE,GPIO_Pin_11);
        GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
				Delayus(500000);
				SampleRes = 300.0;
			}
		}break;
		case Res07:
		{
			if(Current_A<=50)
			{
				ImpandenceStatus = Res08;
				GPIO_ResetBits(GPIOE,GPIO_Pin_8);
        GPIO_SetBits(GPIOE,GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
				Delayus(500000);
				SampleRes = 10000.0;
			}
			else if(Current_A>=250)
			{
				ImpandenceStatus = Res06;
				GPIO_ResetBits(GPIOE,GPIO_Pin_10);
        GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
				Delayus(500000);
				SampleRes = 1000.0;
			}
		}break;
		default:
      break;
	}
}

/**********************************************************************
º¯ÊýÃû³Æ: u16 Sweep(u32 Start_Fre,u32 End_Fre,u16 DAC_Value)
¹¦    ÄÜ: É¨Æµ£º×î´óµçÁ÷·´À¡·¨»ñÈ¡ÏµÍ³Ð³ÕñÆµÂÊÒÔ¼°ÔÚ´ËÐ³ÕñÆµÂÊÏÂ£¨ÊµÊ±ËøÏà£©µÄ×î´óµçÑ¹¡¢×î´óµçÁ÷¡¢²¢»»ËãµÃµ½×è¿¹Öµ£»
Ëµ    Ã÷:
Èë¿Ú²ÎÊý:
·µ »Ø Öµ: ÎÞ
***********************************************************************/
u16 Sweep(u32 Start_Fre,u32 End_Fre,u16 DAC_Value)
{
	u16 i = 0;
	u16 i_flag = 0;
	u16 j_flag = 0;
	u16 k_flag = 0;
	float x = 0.0;
	float y = 0.0;
	u16 MaxAmp = 0;
	unsigned char buf[] = {0};

	u32 Current_Buffer = 0;

	u32 ProgressValue = 0;

	qsize Size = 0;

	FreGain = (End_Fre-Start_Fre)/1000;

	Current_Buffer = (u32)(End_Fre-Start_Fre)/100;

	ImpandenceStatus = Res01;
	SampleRes=5.0;
	angle = 0;

	queue_reset();					//Çå¿Õ´®¿Ú½ÓÊÕ»º³åÇø

	for(Current_Fre=Start_Fre; Current_Fre<=End_Fre;)
	{
		Fre_Buffer[i_flag++] = Current_Fre/1000;
		ad9833_out(Current_Fre/1000, 2);
		Delayus(10000);
	  ADC1_ValueFilter(20);					                                     //²ÉÑùµÃµ½·´À¡µÄµçÑ¹¡¢µçÁ÷Öµ
		Phase_ValueFilter(20);
		Current_ARes = (float)Current_A/SampleRes;

    Size = queue_find_cmd(cmd_buffer,1024);                            //´Ó»º³åÇøÖÐ»ñÈ¡Ò»ÌõÖ¸Áî
		Message_Deal(Size);

		if(Stop_Control_Flag == 1)
		{
			return 1;
		}

		if(Current_Fre > (Start_Fre+Current_Buffer*(ProgressValue+1)))	   //½ø¶ÈÌõ¿ØÖÆ
		{
			ProgressValue++;
			if(ProgressValue<100)
			{
				SetProgressValue(0,24,ProgressValue);
				sprintf((char*)buf,"%d",ProgressValue);
				SetTextValue(0,25,buf);
			}
		}

		if(Current_A >= MaxAmp)	                                           //×î´óµçÁ÷·´À¡·¨£¬É¨ÆµµÃµ½×î´óµçÁ÷
		{
			MaxAmp = Current_A;
			Mid_Fre = Current_Fre/1000;
		}
		//¼ÆËã×è¿¹
		Impandence_Value_Buffer = (double)Current_V*10.1905/((double)Current_ARes/3.582705)*100 - SampleRes*1000;

		if(Impandence_Value_Buffer<1500)
		{
			Impandence_Value_Buffer = Impandence_Value_Buffer*0.910;
		}
		if(Impandence_Value_Buffer<2000 && Impandence_Value_Buffer>=1500)
		{
			Impandence_Value_Buffer = Impandence_Value_Buffer*0.945;
		}
		if(Impandence_Value_Buffer>=2000 && Impandence_Value_Buffer<=2500)
		{
			Impandence_Value_Buffer = Impandence_Value_Buffer*0.958;
		}
		if(Impandence_Value_Buffer<=5000 && Impandence_Value_Buffer>2500)
		{
			Impandence_Value_Buffer = Impandence_Value_Buffer*0.965;
		}
		if(Impandence_Value_Buffer>=3000000)
		{
			Impandence_Value_Buffer = Impandence_Value_Buffer*0.985;
		}

		Impandence_Buffer[Impandence_Buffer_Flag] = Impandence_Value_Buffer;                        //±£´æ×è¿¹
		Impandence_Log10[Impandence_Buffer_Flag] = log10((double)Impandence_Buffer[i_flag])*1000;
		Angle_Buffer[Impandence_Buffer_Flag] = ((angle_P)>>8)|(((angle_P)&0xFF)<<8);                //±£´æÏàÎ»²î

		Angle[Impandence_Buffer_Flag] = (float)(angle_P-1820)/10;
		x = (Angle[Impandence_Buffer_Flag]/180)*PI;    //ÏàÎ»²î×ª»¯Îª»¡¶È
		//¼ÆËãµ¼ÄÉ
		Impandence_G[Impandence_Buffer_Flag] = sqrt((((double)Impandence_Value_Buffer)*((double)Impandence_Value_Buffer))/(1+tan(x)*tan(x)));
		XJ_Temp = Impandence_Value_Buffer - Impandence_G[Impandence_Buffer_Flag];
		Impandence_G_Buffer[Impandence_Buffer_Flag] = Impandence_Value_Buffer + XJ_Temp;            //±£´æµ¼ÄÉ

		if(Impandence_Buffer_Flag==0)
		{
			Angle_Buffer0 = Angle_Buffer[0];
		}

		Impandence_Buffer_Flag++;

		Impandence_Switch();
		Current_Fre = Current_Fre + FreGain;
 	}
	Angle_Buffer[0] = Angle_Buffer0;
	Current_Fre = Current_Fre/1000;

	GPIO_ResetBits(GPIOE,GPIO_Pin_10);
  GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	Delayus(5000000);

	ad9833_out(1000,2);           //1kHzÊä³ö
	Delayus(5000000);
	Delayus(5000000);
	ADC1_ValueFilter(20);					//²ÉÑùµÃµ½·´À¡µÄµçÑ¹¡¢µçÁ÷Öµ

	kkk = (2*PI*1000*(((double)Current_V * 8.34348)/(double)(Current_A/3.420705/1000)));

	CT = (double)(1.0/kkk);       //¼ÆËã1kHzÏÂµÄ×ÔÓÉµçÈÝ

	GPIO_ResetBits(GPIOE,GPIO_Pin_15);
	GPIO_SetBits(GPIOE,GPIO_Pin_10);
	Delayus(5000000);

	/***********************************************************************************/
	for(i=0;i<10;i++)
	{
		Angle_Buffer[i] = Angle_Buffer[10];
		Impandence_Buffer[i] = Impandence_Buffer[10];
	}

	Impandence_Value_Max = Impandence_Buffer[0];
	Impandence_Value = Impandence_Buffer[0];
	for(i_flag=0; i_flag<1000; i_flag++)
	{
		if(Impandence_Buffer[i_flag]<Impandence_Value)
		{
			Impandence_Value = Impandence_Buffer[i_flag];       //ÕÒµ½×îÐ¡×è¿¹
			MinValue_Flag = i_flag;                             //×îÐ¡×è¿¹¶ÔÓ¦µÄÏÂ±ê
		}
		if(Impandence_Buffer[i_flag]>Impandence_Value_Max)
		{
			Impandence_Value_Max = Impandence_Buffer[i_flag];   //ÕÒµ½×î´ó×è¿¹
			MaxValue_Flag = i_flag;                             //×î´ó×è¿¹¶ÔÓ¦µÄÏÂ±ê
		}
	}

	x = (Angle[MinValue_Flag]/180)*PI;           //×îÐ¡×è¿¹¶ÔÓ¦µÄÏàÎ»²î»¡¶ÈÖµ
	//¼ÆËã×îÐ¡×è¿¹
	Impandence_Value_Min = sqrt((((double)Impandence_Value)*((double)Impandence_Value))/(1+tan(x)*tan(x)));
	XJ_Temp = Impandence_Value - Impandence_Value_Min;
	XZ_Impandence = Impandence_Value + XJ_Temp;  //¼ÆËã¶¯Ì¬µç×è

	y = (Angle[MaxValue_Flag]/180)*PI;           //×îÐ¡×è¿¹¶ÔÓ¦µÄÏàÎ»²î»¡¶ÈÖµ
	//¼ÆËã×î´ó×è¿¹
	YJ_Temp = Impandence_Value_Max - sqrt((((double)Impandence_Value_Max)*((double)Impandence_Value_Max))/(1+tan(y)*tan(y)));
	YZ_Impandence = Impandence_Value_Max + YJ_Temp;//¼ÆËã·´Ð³Õñ×è¿¹

	Fre_Max = Fre_Buffer[MaxValue_Flag];           //×î´ó×è¿¹¶ÔÓ¦µÄÆµÂÊ
	Fre_Min = Fre_Buffer[MinValue_Flag];           //×îÐ¡×è¿¹¶ÔÓ¦µÄÆµÂÊ
	Impandence_Log10_Max = log10((double)Impandence_Value_Max)*1000;
	Impandence_Log10_Min = log10((double)Impandence_Value)*1000;

	SetProgressValue(0,24,100);
	sprintf((char*)buf,"%d",100);
	SetTextValue(0,25,buf);
	ProgressValue = 0;
	Impandence_Buffer_Flag = 0;

	k_flag = MinValue_Flag;
	j_flag = MinValue_Flag;
	if((k_flag>0) || (j_flag>0))
	{
		while(Impandence_G_Buffer[k_flag] <= (2*XZ_Impandence) && Impandence_G_Buffer[j_flag] <= (2*XZ_Impandence))	  //Çó°ë¹¦ÂÊµã
		{
			if(k_flag>0)
			{
				k_flag--;
			}

			j_flag++;
			if(Impandence_Value_Max<=2*Impandence_Value)
			{
				break;
			}
		}
	}

	Fre_F1 = Fre_Buffer[k_flag + 1];    //°ë¹¦ÂÊµãF1
	Fre_F2 = Fre_Buffer[j_flag - 1];    //°ë¹¦ÂÊµãF2
	fd = (Fre_F2 - Fre_F1);             //F2-F1

	for(i_flag=0; i_flag<1000; i_flag++)
	{
//		//È¥µô¸ÉÈÅ
//		if (Impandence_Buffer[i_flag + 1] < (Impandence_Buffer[i_flag] * 0.75)  && i_flag < 1000)
//		{
//			Impandence_Buffer[i_flag + 1] = Impandence_Buffer[i_flag] + 1;
//		}

		//½«×è¿¹×ª»¯Îª¶ÔÊýÖµ
		Impandence_Buffer2[i_flag] = ((u16)(log10((double)Impandence_Buffer[i_flag])*1000))>>8|((u16)(log10((double)Impandence_Buffer[i_flag])*1000)&0xff)<<8;
	}

	Qm = (double)Fre_Min/fd;           //¼ÆËãÆ·ÖÊÒòËØ
	L1 = ((double)Impandence_Value/1000)/(2*PI*((double)fd/1000));    //¼ÆËã¶¯Ì¬µç¸Ð
	C1 = (double)1*1000000000/(((double)4*PI*PI*(Fre_Min/1000)*(Fre_Min/1000)*(L1*1000))) * 0.65295;    //¼ÆËã¶¯Ì¬µçÈÝ
	C0 = (CT*10000000000) - C1;       //¼ÆËã¶¯Ì¬µçÈÝ
	//¼ÆËã»úµçñîºÏÏµÊý
	Keff = sqrt(((double)(Fre_Max/1000)*(Fre_Max/1000)-(double)(Fre_Min/1000)*(Fre_Min/1000))/((Fre_Max/1000)*(Fre_Max/1000)));

	sprintf((char*)buf,"%-8.1f",(double)Fre_Max);
	SetTextValue(0,10,buf);     //·´Ð³ÕñÆµÂÊ

	
	sprintf((char*)buf,"%-10.2f",(double)YZ_Impandence/1000000*0.59896);
	SetTextValue(0,20,buf);     //·´Ð³Õñ×è¿¹

	
	sprintf((char*)buf,"%-10.1f",(double)Fre_Min);
	SetTextValue(0,9,buf);      //Ð³ÕñÆµÂÊ
	
	sprintf((char*)buf,"%-10.1f",(double)Fre_F1);
	SetTextValue(0,14,buf);     //°ë¹¦ÂÊµãF1

	
	sprintf((char*)buf,"%-10.1f",(double)Fre_F2);
	SetTextValue(0,15,buf);     //°ë¹¦ÂÊµãF2

	
	sprintf((char*)buf,"%-10.3f",(double)Qm * 1.029);
	SetTextValue(0,12,buf);     //Æ·ÖÊÒòËØ

	
	sprintf((char*)buf,"%-10.4f",(double)Keff * 0.97);
	SetTextValue(0,18,buf);     //keff

	
	sprintf((char*)buf,"%-10.4f",(double)C0);
	SetTextValue(0,16,buf);     //¾²Ì¬µçÈÝ

	
	sprintf((char*)buf,"%-10.4f",(double)C1);
	SetTextValue(0,17,buf);     //¶¯Ì¬µçÈÝ

	
	sprintf((char*)buf,"%-10.3f",(double)L1 * 1.45396);
	SetTextValue(0,21,buf);     //¶¯Ì¬µç¸Ð

	
	sprintf((char*)buf,"%-10.3f",(double)fd);
	SetTextValue(0,19,buf);     //F2 - F1

	
	sprintf((char*)buf,"%-8.2f",(double)XZ_Impandence/1000 * 1.14651);
	SetTextValue(0,11,buf);     //¶¯Ì¬µç×è

	
	sprintf((char*)buf,"%-8.4f",CT*10000000000);
	SetTextValue(0,13,buf);     //×ÔÓÉµçÈÝ


	AnimationPlayFrame(0,2,0);  //¹¤×÷×´Ì¬ÖÃÎ»

	ad9833_out(0,2);            //É¨Æµ½áÊø½ûÖ¹DDSÊä³ö
	AD9833_Init();
	

	return 0;
}

/*****************************************************************
* Ãû    ³Æ£º CampareandAlarm()
* ¹¦    ÄÜ£º ¼ÆËãÖµÓëÉè¶¨Öµ±È½Ï£¬³¬ÏÞ±¨¾¯
* Èë¿Ú²ÎÊý£ºuchar *vfrequent,uchar *vresistance,uchar *vcapacity,uchar *vinductor   
* ³ö¿Ú²ÎÊý£º  0£ºÕý³£·¶Î§ÄÚ      			1£º³¬ÏÞ
 *****************************************************************/

 int CampareandAlarm(double vfrequent,double vresistance,double vcapacity,double vinductor)
 {
 
   if((vfrequent<min_freq)||(vfrequent>max_fre))
   	{
		return 1;
   	}
	else if((vresistance<min_freq)||(vresistance>max_fre))
   	{
		return 1;
   	}
	else if((vcapacity<min_freq)||(vcapacity>max_fre))
   	{
		return 1;
   	}
	else if((vinductor<min_freq)||(vinductor>max_fre))
   	{
		return 1;
   	}
   	else
   		return 0;
  
 } 

/**********************************************************************
º¯ÊýÃû³Æ: void PhaseLock(void)
¹¦    ÄÜ: É¨Æµ£ºÍ¨¹ýÆµÂÊÁ¬¼Ó¡¢ÀÛ¼õ10´ÎÔì³ÉµÄÏàÎ»²î·½ÏòµÄ¸Ä±ä£¬ÅÐ¶¨ÊÇ·ñËøÏà³É¹¦
Ëµ    Ã÷:
Èë¿Ú²ÎÊý:
·µ »Ø Öµ: ÎÞ
***********************************************************************/
void PhaseLock(u32 Start_Fre,u32 End_Fre,u16 Voltage)
{
//	u16 t;
	if (Sweep(Start_Fre, End_Fre, Voltage) == 1)
	{
		Stop_Button();
		Impandence_Buffer_Flag = 0;

		GPIO_ResetBits(GPIOE,GPIO_Pin_15);
    GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);
	}
	else
	{
//		for(t=0;t<1000;t++)
//		{
//			USART2_printf("%d \r\n",Impandence_Buffer2[t]);
//			
//		}
//		for(t=0;t<800;t++)
//		{
//			USART2_printf("%d \r\n",Angle_Buffer[t]);
//			
//		}
		GraphSetViewport(0,23,0,33,0,5);    //ÏàÎ»ÇúÏß
		GraphSetViewport(0,33,0,33,Impandence_Log10_Max-(185*(Impandence_Log10_Max-Impandence_Log10_Min)/180),18000/(Impandence_Log10_Max-Impandence_Log10_Min));		 //5-185
		TIM_Cmd(TIM2, ENABLE);

		while (1)
		{
			if ((Time_100Ms>0)&&(Time_100Ms<=1))
			{
				GraphChannelDataInsert(0,23,0,(u8*)Angle_Buffer,2000);
			}
			else if ((Time_100Ms>1) && (Time_100Ms<3))
			{
				GraphChannelDataInsert(0,33,0,(u8*)Impandence_Buffer2,2000);
			}
			else if (Time_100Ms >= 3)
			{
				Time_100Ms = 0;
				break;
			}
		}
		if(CampareandAlarm((double)Fre_Min,(double)XZ_Impandence/1000 * 1.14651,(double)C0,(double)L1 * 1.45396))
			{
				SetTextValue(0,8,"12");     //ÏÔÊ¾³¬ÏÞÐÅÏ¢
		}
		TIM_Cmd(TIM2, DISABLE);
		
	}
}

/**********************************************************************
º¯ÊýÃû³Æ: void USART2_printf (char *fmt,...)
¹¦    ÄÜ: ÊµÏÖÀàËÆprintf¹¦ÄÜ
Ëµ    Ã÷:
Èë¿Ú²ÎÊý:
·µ »Ø Öµ: ÎÞ
***********************************************************************/
void USART2_printf (char *fmt,...)
{
	char buffer[254+1]; 
	u8 i = 0;
	va_list arg_ptr;
	va_start(arg_ptr, fmt); 
	vsnprintf(buffer, 254+1, fmt, arg_ptr);
	while ((i < 255) && buffer[i])
	{
		USART_SendData(USART2, (u8) buffer[i++]);
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 
	}
	va_end(arg_ptr);
}

/**********************************************************************
º¯ÊýÃû³Æ:void Send_Space(u8 t)
¹¦    ÄÜ: ´®¿Ú2·¢ËÍ¿Õ¸ñ£¬
Ëµ    Ã÷:
Èë¿Ú²ÎÊý: t£º ¿Õ¸ñ¸öÊý
·µ »Ø Öµ: ÎÞ
***********************************************************************/

void Send_Space(u8 t)
{
	u8 i=0;
	for(i=0;i<t;i++)
		USART2_printf(" ");
	
}
/**********************************************************************
º¯ÊýÃû³Æ:void USART2_IRQHandler()
¹¦    ÄÜ: ´®¿Ú2½ÓÊÕÖÐ¶Ï
Ëµ    Ã÷:
Èë¿Ú²ÎÊý:
·µ »Ø Öµ: ÎÞ
***********************************************************************/
void USART2_IRQHandler()
{


	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  
	{

		USART2_RX =USART_ReceiveData(USART2);	//¶ÁÈ¡½ÓÊÕµ½µÄÊý¾Ý  
	}
}

/**********************************************************************
º¯ÊýÃû³Æ:void Send_Data_USB()
¹¦    ÄÜ: ·¢ËÍÊý¾Ýµ½UÅÌµÄµ¥Æ¬»ú
Ëµ    Ã÷:
Èë¿Ú²ÎÊý: ÎÞ
·µ »Ø Öµ: ÎÞ
***********************************************************************/

void Send_Data_USB()
{  	
	u16 t=0;
	unsigned char buf[] = {0};
	u16 i=0;	
	
	ShowControl(0,28,0); 	
	USART2->DR=0x54;
	while((USART2->SR&0X40)==0x00);//µÈ´ý·¢ËÍ½áÊø
	
	//·¢ÎÄ¼þÃû
	USART2_printf("%d",file_name);    //DGUT//
	USART2->DR=0x56;
	while((USART2->SR&0X40)==0x00);//µÈ´ý·¢ËÍ½áÊø
	//
	sprintf((char*)buf,"%-10.1f",(double)Fre_Min);	//Ð³ÕñÆµÂÊ
	USART2_printf("%S",buf);    //DGUT//
	Send_Space(2);

	sprintf((char*)buf,"%-8.2f",(double)XZ_Impandence/1000 * 1.14651);//¶¯Ì¬µç×è
	USART2_printf("%S",buf);    //DGUT//
	Send_Space(2);

	sprintf((char*)buf,"%-10.1f",(double)Fre_F1);//°ë¹¦ÂÊµãF1
	USART2_printf("%S",buf);    //DGUT//
	
	 USART2->DR=0x56;
	 while((USART2->SR&0X40)==0);//µÈ´ý·¢ËÍ½áÊø			µÚÒ»ÐÐ

	sprintf((char*)buf,"%-8.1f",(double)Fre_Max);//·´Ð³ÕñÆµÂÊ
	USART2_printf("%S",buf);    //DGUT//
	Send_Space(3);
	
	sprintf((char*)buf,"%-10.2f",(double)YZ_Impandence/1000000*0.59896); //·´Ð³Õñ×è¿¹
	USART2_printf("%S",buf);    //DGUT//
	Send_Space(1);
	
	sprintf((char*)buf,"%-10.1f",(double)Fre_F2); //°ë¹¦ÂÊµãF2
	USART2_printf("%S",buf);    //DGUT//
	
	USART2->DR=0x56;
	 while((USART2->SR&0X40)==0);//µÈ´ý·¢ËÍ½áÊø		µÚ¶þÐÐ
	
	sprintf((char*)buf,"%-10.3f",(double)Qm * 1.029);//Æ·ÖÊÒòËØ
	USART2_printf("%S",buf);    //DGUT//
	Send_Space(1);
	
	sprintf((char*)buf,"%-10.4f",(double)Keff * 0.97); //keff	
	USART2_printf("%S",buf);    //DGUT// 
	Send_Space(1);


	sprintf((char*)buf,"%-10.3f",(double)fd);
	USART2_printf("%S",buf);    //DGUT//

	 USART2->DR=0x56;
	 while((USART2->SR&0X40)==0);//µÈ´ý·¢ËÍ½áÊø		µÚÈýÐÐ
	 
	 
	sprintf((char*)buf,"%-8.4f",CT*10000000000);//×ÔÓÉµçÈÝ
	USART2_printf("%S",buf);    //DGUT//
	Send_Space(3);

	sprintf((char*)buf,"%-10.4f",(double)C1);     //¶¯Ì¬µçÈÝ
	USART2_printf("%S",buf);    //DGUT// 
	Send_Space(1);

	sprintf((char*)buf,"%-10.4f",(double)C0);//¾²Ì¬µçÈÝ
	USART2_printf("%S",buf);    //DGUT//

	 USART2->DR=0x56;
	 while((USART2->SR&0X40)==0);//µÈ´ý·¢ËÍ½áÊø			µÚËÄÐÐ
	 
	sprintf((char*)buf,"%-10.3f",(double)L1 * 1.45396); //¶¯Ì¬µç¸Ð
	USART2_printf("%S",buf);    //DGUT//
	
	USART2->DR=0x56;
	while((USART2->SR&0X40)==0x00) {}; //µÈ´ý·¢ËÍ½áÊø
	USART2->DR=0x55;
	while((USART2->SR&0X40)==0x00);//µÈ´ý·¢ËÍ½áÊø
	USART2->DR=0x55;
	while((USART2->SR&0X40)==0x00){};//µÈ´ý·¢ËÍ½áÊø

    TIM_Cmd(TIM2, ENABLE);
		
//·¢ËÍ3¸öÊý×é
	while(1)
	{
        if(Time_100Ms>300)          //Èç¹û²»Õý³££¬30sºóÍË³öËÀÑ­»·
        {
            t=0;
            USART2_RX=0;
            SetScreen(2);           //ÏÔÊ¾±£´æ³ÉÊ§°Ü½çÃæ
            Beep_On();              //¿ª·äÃùÆ÷
            Delayus(400000);
            Beep_Off();             //¹Ø·äÃùÆ÷  
            Delayus(4000000);
            Delayus(4000000);
            SetScreen(0);	        //ÏÔÊ¾Ö÷½çÃæ
            break;	            
        }
		if(t>=67)
		{
				SetProgressValue(0,24,100);
				sprintf((char*)buf,"%d",100);
				SetTextValue(0,25,buf);
                Delayus(4000000);
                if(USART2_RX==0x59)       //½ÓÊÕµ½³É¹¦±£´æÊý¾ÝµÄ±êÖ¾Y
                {
                      USART2_RX=0;
                      SetScreen(3);	     //ÏÔÊ±±£´æ³É¹¦µÄ½çÃæ
                      Beep_On();         //¿ª·äÃùÆ÷
                      Delayus(400000);
                      Beep_Off();        //¹Ø·äÃùÆ÷ 
                      t=0;      	
                      Delayus(4000000);
                      Delayus(4000000);
                      Delayus(4000000);
                      SetScreen(0);	     //ÏÔÊ¾Ö÷½çÃæ   
                      break;
                }
                else if(USART2_RX==0x58)    //UÅÌ´æ´¢³öÏÖÎÊÌâ
                {
                      t=0;
                      USART2_RX=0;
                      SetScreen(2);		  //ÏÔÊ¾±£´æÊ§°Ü½çÃæ
                      Beep_On();          //¿ª·äÃùÆ÷
                      Delayus(400000);
                      Beep_Off();         //¹Ø·äÃùÆ÷   
                      Delayus(4000000);
                      Delayus(4000000);
                      SetScreen(0);	      //ÏÔÊ¾Ö÷½çÃæ                    
                      break;
                }   
		}    
		if(USART2_RX==0x51)
		{	   
			USART2->DR=0x54;                   //·¢ËÍÆðÊ¼·ûT
			while((USART2->SR&0X40)==0x00) {}; //µÈ´ý·¢ËÍ½áÊø
					
			for(i=t*15;i<(t*15+15);i++)
			{
				USART2_printf("%d   ",Fre_Buffer[i]);               //·¢ËÍÆµÂÊ
				USART2_printf("%d    ",Impandence_Buffer[i]);       //·¢ËÍ×è¿¹
				USART2_printf("%f   \r\n",Angle[i]);		        //·¢ËÍÏàÎ»		
			}
			t++;
			USART2->DR=0x55;                     //·¢ËÍ½áÊø·ûºÅU          
			while((USART2->SR&0X40)==0x00){};   //µÈ´ý·¢ËÍ½áÊø
			USART2->DR=0x55;
			while((USART2->SR&0X40)==0x00){};   //µÈ´ý·¢ËÍ½áÊø
            USART2_RX=0;
            SetProgressValue(0,24,t*100/68);
            sprintf((char*)buf,"%d",t*100/68);
            SetTextValue(0,25,buf); 	  
		}		
	} 
    Time_100Ms=0;
	ShowControl(0,28,1);       	//ÏÔÊ¾±£´æ°´Å¥	
    TIM_Cmd(TIM2, DISABLE);     //¹Ø±Õ¶¨Ê±Æ÷2
}

/**********************************************************************
º¯ÊýÃû³Æ:void Send_Data_PC()
¹¦    ÄÜ: ·¢ËÍÊý¾Ýµ½ÉÏÎ»»ú
Ëµ    Ã÷:
Èë¿Ú²ÎÊý: ÎÞ
·µ »Ø Öµ: ÎÞ
***********************************************************************/
void Send_Data_PC()
{ 
    u8 Check=0;
	unsigned char buf[] = {0};
    u16 i;
	
    USART2_printf("%c",'Z');        //·¢ËÍÆðÊ¼·ûZ
	USART2->DR=0x0f;                //·¢ËÍÓÒ±ß±êÖ¾
	while((USART2->SR&0X40)==0){};  
       
	sprintf((char*)buf,"%-10.1f",(double)Fre_Min);	//Ð³ÕñÆµÂÊ
	USART2_printf("%S",buf);    //DGUT//
    USART2_printf("%c",'V');    //Ã¿¸öÊý×ÖµÄ·Ö¸ô·û
    Check=buf[0];

	sprintf((char*)buf,"%-8.2f",(double)XZ_Impandence/1000 * 1.14651);//¶¯Ì¬µç×è
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;
    USART2_printf("%c",'V');    //Ã¿¸öÊý×ÖµÄ·Ö¸ô·û

	sprintf((char*)buf,"%-10.1f",(double)Fre_F1);//°ë¹¦ÂÊµãF1
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //Ã¿¸öÊý×ÖµÄ·Ö¸ô·û

	sprintf((char*)buf,"%-8.1f",(double)Fre_Max);//·´Ð³ÕñÆµÂÊ
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //Ã¿¸öÊý×ÖµÄ·Ö¸ô·û
	
	sprintf((char*)buf,"%-10.2f",(double)YZ_Impandence/1000000*0.59896); //·´Ð³Õñ×è¿¹
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //Ã¿¸öÊý×ÖµÄ·Ö¸ô·û
	
	sprintf((char*)buf,"%-10.1f",(double)Fre_F2); //°ë¹¦ÂÊµãF2
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //Ã¿¸öÊý×ÖµÄ·Ö¸ô·û
	
	sprintf((char*)buf,"%-10.3f",(double)Qm * 1.029);//Æ·ÖÊÒòËØ
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //Ã¿¸öÊý×ÖµÄ·Ö¸ô·û
	
	sprintf((char*)buf,"%-10.4f",(double)Keff * 0.97); //keff	
	USART2_printf("%S ",buf);    //DGUT// 
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //Ã¿¸öÊý×ÖµÄ·Ö¸ô·û

	sprintf((char*)buf,"%-10.3f",(double)fd);
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //Ã¿¸öÊý×ÖµÄ·Ö¸ô·û
	 
	 
	sprintf((char*)buf,"%-8.4f",CT*10000000000);//×ÔÓÉµçÈÝ
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //Ã¿¸öÊý×ÖµÄ·Ö¸ô·û

	sprintf((char*)buf,"%-10.4f",(double)C1);     //¶¯Ì¬µçÈÝ
	USART2_printf("%S ",buf);    //DGUT// 
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //Ã¿¸öÊý×ÖµÄ·Ö¸ô·û

	sprintf((char*)buf,"%-10.4f",(double)C0);//¾²Ì¬µçÈÝ
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //Ã¿¸öÊý×ÖµÄ·Ö¸ô·û
	 
	sprintf((char*)buf,"%-10.3f",(double)L1 * 1.45396); //¶¯Ì¬µç¸Ð
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');                //Ã¿¸öÊý×ÖµÄ·Ö¸ô·û
    USART_SendData(USART2,'W');             //Ò»Ö¡µÄ·¢ËÍ½áÊø·û 
    USART_SendData(USART2,Check);           //Ð£ÑéÂë    
    Delayus(1000);
      
    for(i=0;i<1000;i++)
    {
        USART2_printf("%c",'Z');                   //·¢ËÍÆðÊ¼·ûZ
        USART_SendData(USART2,0xf0);               //·¢ËÍ×ó±ß±êÖ¾Î»

        USART2_printf("%ld",Fre_Buffer[i]);
        USART2_printf("%c",'V');                //Ã¿¸öÊý×ÖµÄ·Ö¸ô·û
        sprintf((char*)buf,"%-10.1f",(double)Fre_Buffer[i]);	
        Check=buf[0];

        USART2_printf("%ld",Impandence_Buffer[i]);
        USART2_printf("%c",'V');                //Ã¿¸öÊý×ÖµÄ·Ö¸ô·û
        sprintf((char*)buf,"%-10.1f",(double)Impandence_Buffer[i]);	
        Check=Check&buf[0];
        
        USART2_printf("%f",Angle[i]);
        USART2_printf("%c",'V');                //Ã¿¸öÊý×ÖµÄ·Ö¸ô·û
        sprintf((char*)buf,"%-10.1f",(double)Angle[i]);	
        Check=Check&buf[0];
       
        
        USART2_printf("%c",'W');            //Ò»Ö¡µÄ·¢ËÍ½áÊø·û
        USART_SendData(USART2,Check);      //Ð£ÑéÂë  
    }
}

