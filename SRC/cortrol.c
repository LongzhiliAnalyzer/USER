/*********************************************************************
*                Copyright (C), 2015-2016, Supersonics. Co., Ltd.
*                        �迹��������������
*
*                          Ӳ��ƽ̨: xxx
*                          �� о Ƭ: STM32F103
*                          �� Ŀ ��: xxx
**********************************************************************
*�ļ���: control.c
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

#include "control.h"
#include "CtrFile.h"

#include "cmd_queue.h"
#include "AD9833.h"
#include <math.h>
#include <stdio.h>

#include <stdarg.h>

u8 USART2_RX=0;

u32 Current_Fre=0;	//��ǰƵ��
u16 Current_V = 0;	//��ǰ�ɼ��ĵ�ѹֵ
u16 Current_A = 0;	//��ǰ�ɼ��ĵ���ֵ
u32 Impandence_Value = 0;//�迹ֵ

u32 Impandence_Value_Buffer=1500;

u32 FreGain = 5.0;                               /*ɨƵ�����ı���*/

u16 ImpandenceStatus = Res01;                    /*�迹�л�ʱ������״̬ѡ��*/
float SampleRes=5.0;                             /*�迹���*/
u32 Mid_Fre = 0;			                           /*���岢��ʼ��г��Ƶ�ʵ�*/
float Current_ARes =0.0;                         /*�������*/

s32 angle_P = 0;

u32 Impandence_Buffer[2048] = {0};               /*����һ��������飬���ڱ���ɨƵ�����еõ����迹ֵ*/
u16 Impandence_Buffer2[2048] = {0};              /*����һ��������飬���ڱ������ת������迹ֵ*/
u16 Impandence_Log10[2048] = {0};
s16 Angle_Buffer[2048] = {0};                    /*����һ��������飬���ڱ���ɨƵ�����еõ�����λֵ��δ��ת����*/
float Angle[2048] = {0.0};                       /*����һ��������飬���ڱ���ɨƵ�����еõ�����λֵ*/
u32 Fre_Buffer[2048] = {0};                      /*����ɨƵ�����е�Ƶ�ʵ���*/
u16 Impandence_Buffer_Flag = 0;                  /*���ڼ�¼ɨƵ��λ��*/
u32 Impandence_Value_Max = 0;                    /*����迹ֵ*/
u32 Impandence_Value_Min = 0;                    /*��С�迹ֵ*/
u32 Impandence_G[2048] = {0};                    /*   */
u32 Impandence_G_Buffer[2048] = {0};             /*   */
u32 XJ_Temp = 0;                                 /*   */
u32 XZ_Impandence = 0;                           /*   */
u32 YJ_Temp = 0;                                 /*   */
u32 YZ_Impandence = 0;                           /*   */
u16 Impandence_Log10_Max = 0;                    /*����ת���Ժ���迹���ֵ*/
u16 Impandence_Log10_Min = 0;                    /*����ת���Ժ���迹��Сֵ*/
u32 Fre_Max = 0;                                 /*��г���迹*/
u32 Fre_Min = 0;                                 /*г���迹*/
s16 Angle_Buffer0 = 0;

u16 MaxValue_Flag = 0;
u16 MinValue_Flag = 0;


double kkk = 0.0;
double CT = 0.0;                                 /*���ڼ��㾲̬����C0��ת������*/

double Qm = 0.0;                                 /*���岢��ʼ����еƷ������Qm*/
double L1 = 0.0;                                 /*���岢��ʼ����̬���L1*/
double C1 = 0.0;                                 /*���岢��ʼ����̬����C1*/
double C0 = 0.0;                                 /*���岢��ʼ����̬����C1*/
double Keff = 0.0;                               /*���岢��ʼ���������ϵ��*/
u32 Fre_F1 = 0;			       //���岢��ʼ���빦�ʵ�F1
u32 Fre_F2 = 0;				     //���岢��ʼ���빦�ʵ�F2
u32 fd = 0;

//u32 file_name=0;         //DGUT
/******************************************************************
* �������ã������ݽ�����ֵ�˲�
* ����������1������ָ�룻2���˲�����
* ��������ֵ���˲�����м���ֵ
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
* �������ã������ݽ��о�ֵ�˲�
* ����������1������ָ�룻2���˲�����
* ��������ֵ���˲����ƽ����ֵ
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
* �������ã������ݽ�����ֵƽ���˲�����num���ɼ����ݣ�ȥ�������Сֵ����������ƽ��ֵ
* ����������1������ָ�룻2���˲�����
* ��������ֵ���˲����ƽ����ֵ
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
�������ƣ�void Phase_ValueFilter(u8 num)
��    �ܣ��˲�������,����λ�����˲�����
˵    ����
��ڲ������˲������?um(0��1��2��3...20)
����ֵ  ����
********************************************************************/
void Phase_ValueFilter(u8 num)
{
	u16 i = 0;
	s16 Phase[30] = {0};

	TIM4->CCER &= (uint16_t)~((uint16_t)TIM_CCER_CC2E);	         //��ֹ��ͨ������
	TIM4->CCER |= (uint16_t)TIM_CCER_CC1E;				               //����һͨ������

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
�������ƣ�void ADC1_ValueFilter(u8 num) �����ǵõ������ĵ�ѹ������ֵ
��    �ܣ��˲������� //������������ֵ�˲����õ��ĵ�ѹ�͵���ֵ��
˵    ����
��ڲ������˲������?um(0��1��2��3...20)
����ֵ  ����
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
��������: void Send_Cmd(void)
��    ��: ����CT����ֵ������ָ��
˵    ��:
��ڲ���:
�� �� ֵ: ��
***********************************************************************/
void Send_Cmd(void)
{
	u8 Parity = 0;
	u32 Capacitor = 0;
	Capacitor = CT*1000000;

	USART_SendData(USART1, 0x3C);  //����ͬ���ַ�
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
* �������ã��迹�л��жϺ���
* ������������
* ��������ֵ����
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
��������: u16 Sweep(u32 Start_Fre,u32 End_Fre,u16 DAC_Value)
��    ��: ɨƵ����������������ȡϵͳг��Ƶ���Լ��ڴ�г��Ƶ���£�ʵʱ���ࣩ������ѹ����������������õ��迹ֵ��
˵    ��:
��ڲ���:
�� �� ֵ: ��
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

	queue_reset();					//��մ��ڽ��ջ�����

	for(Current_Fre=Start_Fre; Current_Fre<=End_Fre;)
	{
		Fre_Buffer[i_flag++] = Current_Fre/1000;
		ad9833_out(Current_Fre/1000, 2);
		Delayus(10000);
	  ADC1_ValueFilter(20);					                                     //�����õ������ĵ�ѹ������ֵ
		Phase_ValueFilter(20);
		Current_ARes = (float)Current_A/SampleRes;

    Size = queue_find_cmd(cmd_buffer,1024);                            //�ӻ������л�ȡһ��ָ��
		Message_Deal(Size);

		if(Stop_Control_Flag == 1)
		{
			return 1;
		}

		if(Current_Fre > (Start_Fre+Current_Buffer*(ProgressValue+1)))	   //����������
		{
			ProgressValue++;
			if(ProgressValue<100)
			{
				SetProgressValue(0,24,ProgressValue);
				sprintf((char*)buf,"%d",ProgressValue);
				SetTextValue(0,25,buf);
			}
		}

		if(Current_A >= MaxAmp)	                                           //��������������ɨƵ�õ�������
		{
			MaxAmp = Current_A;
			Mid_Fre = Current_Fre/1000;
		}
		//�����迹
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

		Impandence_Buffer[Impandence_Buffer_Flag] = Impandence_Value_Buffer;                        //�����迹
		Impandence_Log10[Impandence_Buffer_Flag] = log10((double)Impandence_Buffer[i_flag])*1000;
		Angle_Buffer[Impandence_Buffer_Flag] = ((angle_P)>>8)|(((angle_P)&0xFF)<<8);                //������λ��

		Angle[Impandence_Buffer_Flag] = (float)(angle_P-1820)/10;
		x = (Angle[Impandence_Buffer_Flag]/180)*PI;    //��λ��ת��Ϊ����
		//���㵼��
		Impandence_G[Impandence_Buffer_Flag] = sqrt((((double)Impandence_Value_Buffer)*((double)Impandence_Value_Buffer))/(1+tan(x)*tan(x)));
		XJ_Temp = Impandence_Value_Buffer - Impandence_G[Impandence_Buffer_Flag];
		Impandence_G_Buffer[Impandence_Buffer_Flag] = Impandence_Value_Buffer + XJ_Temp;            //���浼��

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

	ad9833_out(1000,2);           //1kHz���
	Delayus(5000000);
	Delayus(5000000);
	ADC1_ValueFilter(20);					//�����õ������ĵ�ѹ������ֵ

	kkk = (2*PI*1000*(((double)Current_V * 8.34348)/(double)(Current_A/3.420705/1000)));

	CT = (double)(1.0/kkk);       //����1kHz�µ����ɵ���

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
			Impandence_Value = Impandence_Buffer[i_flag];       //�ҵ���С�迹
			MinValue_Flag = i_flag;                             //��С�迹��Ӧ���±�
		}
		if(Impandence_Buffer[i_flag]>Impandence_Value_Max)
		{
			Impandence_Value_Max = Impandence_Buffer[i_flag];   //�ҵ�����迹
			MaxValue_Flag = i_flag;                             //����迹��Ӧ���±�
		}
	}

	x = (Angle[MinValue_Flag]/180)*PI;           //��С�迹��Ӧ����λ���ֵ
	//������С�迹
	Impandence_Value_Min = sqrt((((double)Impandence_Value)*((double)Impandence_Value))/(1+tan(x)*tan(x)));
	XJ_Temp = Impandence_Value - Impandence_Value_Min;
	XZ_Impandence = Impandence_Value + XJ_Temp;  //���㶯̬����

	y = (Angle[MaxValue_Flag]/180)*PI;           //��С�迹��Ӧ����λ���ֵ
	//��������迹
	YJ_Temp = Impandence_Value_Max - sqrt((((double)Impandence_Value_Max)*((double)Impandence_Value_Max))/(1+tan(y)*tan(y)));
	YZ_Impandence = Impandence_Value_Max + YJ_Temp;//���㷴г���迹

	Fre_Max = Fre_Buffer[MaxValue_Flag];           //����迹��Ӧ��Ƶ��
	Fre_Min = Fre_Buffer[MinValue_Flag];           //��С�迹��Ӧ��Ƶ��
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
		while(Impandence_G_Buffer[k_flag] <= (2*XZ_Impandence) && Impandence_G_Buffer[j_flag] <= (2*XZ_Impandence))	  //��빦�ʵ�
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

	Fre_F1 = Fre_Buffer[k_flag + 1];    //�빦�ʵ�F1
	Fre_F2 = Fre_Buffer[j_flag - 1];    //�빦�ʵ�F2
	fd = (Fre_F2 - Fre_F1);             //F2-F1

	for(i_flag=0; i_flag<1000; i_flag++)
	{
//		//ȥ������
//		if (Impandence_Buffer[i_flag + 1] < (Impandence_Buffer[i_flag] * 0.75)  && i_flag < 1000)
//		{
//			Impandence_Buffer[i_flag + 1] = Impandence_Buffer[i_flag] + 1;
//		}

		//���迹ת��Ϊ����ֵ
		Impandence_Buffer2[i_flag] = ((u16)(log10((double)Impandence_Buffer[i_flag])*1000))>>8|((u16)(log10((double)Impandence_Buffer[i_flag])*1000)&0xff)<<8;
	}

	Qm = (double)Fre_Min/fd;           //����Ʒ������
	L1 = ((double)Impandence_Value/1000)/(2*PI*((double)fd/1000));    //���㶯̬���
	C1 = (double)1*1000000000/(((double)4*PI*PI*(Fre_Min/1000)*(Fre_Min/1000)*(L1*1000))) * 0.65295;    //���㶯̬����
	C0 = (CT*10000000000) - C1;       //���㶯̬����
	//����������ϵ��
	Keff = sqrt(((double)(Fre_Max/1000)*(Fre_Max/1000)-(double)(Fre_Min/1000)*(Fre_Min/1000))/((Fre_Max/1000)*(Fre_Max/1000)));

	sprintf((char*)buf,"%-8.1f",(double)Fre_Max);
	SetTextValue(0,10,buf);     //��г��Ƶ��

	
	sprintf((char*)buf,"%-10.2f",(double)YZ_Impandence/1000000*0.59896);
	SetTextValue(0,20,buf);     //��г���迹

	
	sprintf((char*)buf,"%-10.1f",(double)Fre_Min);
	SetTextValue(0,9,buf);      //г��Ƶ��
	
	sprintf((char*)buf,"%-10.1f",(double)Fre_F1);
	SetTextValue(0,14,buf);     //�빦�ʵ�F1

	
	sprintf((char*)buf,"%-10.1f",(double)Fre_F2);
	SetTextValue(0,15,buf);     //�빦�ʵ�F2

	
	sprintf((char*)buf,"%-10.3f",(double)Qm * 1.029);
	SetTextValue(0,12,buf);     //Ʒ������

	
	sprintf((char*)buf,"%-10.4f",(double)Keff * 0.97);
	SetTextValue(0,18,buf);     //keff

	
	sprintf((char*)buf,"%-10.4f",(double)C0);
	SetTextValue(0,16,buf);     //��̬����

	
	sprintf((char*)buf,"%-10.4f",(double)C1);
	SetTextValue(0,17,buf);     //��̬����

	
	sprintf((char*)buf,"%-10.3f",(double)L1 * 1.45396);
	SetTextValue(0,21,buf);     //��̬���

	
	sprintf((char*)buf,"%-10.3f",(double)fd);
	SetTextValue(0,19,buf);     //F2 - F1

	
	sprintf((char*)buf,"%-8.2f",(double)XZ_Impandence/1000 * 1.14651);
	SetTextValue(0,11,buf);     //��̬����

	
	sprintf((char*)buf,"%-8.4f",CT*10000000000);
	SetTextValue(0,13,buf);     //���ɵ���


	AnimationPlayFrame(0,2,0);  //����״̬��λ

	ad9833_out(0,2);            //ɨƵ������ֹDDS���
	AD9833_Init();
	

	return 0;
}

/*****************************************************************
* ��    �ƣ� CampareandAlarm()
* ��    �ܣ� ����ֵ���趨ֵ�Ƚϣ����ޱ���
* ��ڲ�����uchar *vfrequent,uchar *vresistance,uchar *vcapacity,uchar *vinductor   
* ���ڲ�����  0��������Χ��      			1������
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
��������: void PhaseLock(void)
��    ��: ɨƵ��ͨ��Ƶ�����ӡ��ۼ�10����ɵ���λ���ĸı䣬�ж��Ƿ�����ɹ�
˵    ��:
��ڲ���:
�� �� ֵ: ��
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
		GraphSetViewport(0,23,0,33,0,5);    //��λ����
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
				SetTextValue(0,8,"12");     //��ʾ������Ϣ
		}
		TIM_Cmd(TIM2, DISABLE);
		
	}
}

/**********************************************************************
��������: void USART2_printf (char *fmt,...)
��    ��: ʵ������printf����
˵    ��:
��ڲ���:
�� �� ֵ: ��
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
��������:void Send_Space(u8 t)
��    ��: ����2���Ϳո�
˵    ��:
��ڲ���: t�� �ո����
�� �� ֵ: ��
***********************************************************************/

void Send_Space(u8 t)
{
	u8 i=0;
	for(i=0;i<t;i++)
		USART2_printf(" ");
	
}
/**********************************************************************
��������:void USART2_IRQHandler()
��    ��: ����2�����ж�
˵    ��:
��ڲ���:
�� �� ֵ: ��
***********************************************************************/
void USART2_IRQHandler()
{


	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  
	{

		USART2_RX =USART_ReceiveData(USART2);	//��ȡ���յ�������  
	}
}

/**********************************************************************
��������:void Send_Data_USB()
��    ��: �������ݵ�U�̵ĵ�Ƭ��
˵    ��:
��ڲ���: ��
�� �� ֵ: ��
***********************************************************************/

void Send_Data_USB()
{  	
	u16 t=0;
	unsigned char buf[] = {0};
	u16 i=0;	
	
	ShowControl(0,28,0); 	
	USART2->DR=0x54;
	while((USART2->SR&0X40)==0x00);//�ȴ����ͽ���
	
	//���ļ���
	USART2_printf("%d",file_name);    //DGUT//
	USART2->DR=0x56;
	while((USART2->SR&0X40)==0x00);//�ȴ����ͽ���
	//
	sprintf((char*)buf,"%-10.1f",(double)Fre_Min);	//г��Ƶ��
	USART2_printf("%S",buf);    //DGUT//
	Send_Space(2);

	sprintf((char*)buf,"%-8.2f",(double)XZ_Impandence/1000 * 1.14651);//��̬����
	USART2_printf("%S",buf);    //DGUT//
	Send_Space(2);

	sprintf((char*)buf,"%-10.1f",(double)Fre_F1);//�빦�ʵ�F1
	USART2_printf("%S",buf);    //DGUT//
	
	 USART2->DR=0x56;
	 while((USART2->SR&0X40)==0);//�ȴ����ͽ���			��һ��

	sprintf((char*)buf,"%-8.1f",(double)Fre_Max);//��г��Ƶ��
	USART2_printf("%S",buf);    //DGUT//
	Send_Space(3);
	
	sprintf((char*)buf,"%-10.2f",(double)YZ_Impandence/1000000*0.59896); //��г���迹
	USART2_printf("%S",buf);    //DGUT//
	Send_Space(1);
	
	sprintf((char*)buf,"%-10.1f",(double)Fre_F2); //�빦�ʵ�F2
	USART2_printf("%S",buf);    //DGUT//
	
	USART2->DR=0x56;
	 while((USART2->SR&0X40)==0);//�ȴ����ͽ���		�ڶ���
	
	sprintf((char*)buf,"%-10.3f",(double)Qm * 1.029);//Ʒ������
	USART2_printf("%S",buf);    //DGUT//
	Send_Space(1);
	
	sprintf((char*)buf,"%-10.4f",(double)Keff * 0.97); //keff	
	USART2_printf("%S",buf);    //DGUT// 
	Send_Space(1);


	sprintf((char*)buf,"%-10.3f",(double)fd);
	USART2_printf("%S",buf);    //DGUT//

	 USART2->DR=0x56;
	 while((USART2->SR&0X40)==0);//�ȴ����ͽ���		������
	 
	 
	sprintf((char*)buf,"%-8.4f",CT*10000000000);//���ɵ���
	USART2_printf("%S",buf);    //DGUT//
	Send_Space(3);

	sprintf((char*)buf,"%-10.4f",(double)C1);     //��̬����
	USART2_printf("%S",buf);    //DGUT// 
	Send_Space(1);

	sprintf((char*)buf,"%-10.4f",(double)C0);//��̬����
	USART2_printf("%S",buf);    //DGUT//

	 USART2->DR=0x56;
	 while((USART2->SR&0X40)==0);//�ȴ����ͽ���			������
	 
	sprintf((char*)buf,"%-10.3f",(double)L1 * 1.45396); //��̬���
	USART2_printf("%S",buf);    //DGUT//
	
	USART2->DR=0x56;
	while((USART2->SR&0X40)==0x00) {}; //�ȴ����ͽ���
	USART2->DR=0x55;
	while((USART2->SR&0X40)==0x00);//�ȴ����ͽ���
	USART2->DR=0x55;
	while((USART2->SR&0X40)==0x00){};//�ȴ����ͽ���

    TIM_Cmd(TIM2, ENABLE);
		
//����3������
	while(1)
	{
        if(Time_100Ms>300)          //�����������30s���˳���ѭ��
        {
            t=0;
            USART2_RX=0;
            SetScreen(2);           //��ʾ�����ʧ�ܽ���
            Beep_On();              //��������
            Delayus(400000);
            Beep_Off();             //�ط�����  
            Delayus(4000000);
            Delayus(4000000);
            SetScreen(0);	        //��ʾ������
            break;	            
        }
		if(t>=67)
		{
				SetProgressValue(0,24,100);
				sprintf((char*)buf,"%d",100);
				SetTextValue(0,25,buf);
                Delayus(4000000);
                if(USART2_RX==0x59)       //���յ��ɹ��������ݵı�־Y
                {
                      USART2_RX=0;
                      SetScreen(3);	     //��ʱ����ɹ��Ľ���
                      Beep_On();         //��������
                      Delayus(400000);
                      Beep_Off();        //�ط����� 
                      t=0;      	
                      Delayus(4000000);
                      Delayus(4000000);
                      Delayus(4000000);
                      SetScreen(0);	     //��ʾ������   
                      break;
                }
                else if(USART2_RX==0x58)    //U�̴洢��������
                {
                      t=0;
                      USART2_RX=0;
                      SetScreen(2);		  //��ʾ����ʧ�ܽ���
                      Beep_On();          //��������
                      Delayus(400000);
                      Beep_Off();         //�ط�����   
                      Delayus(4000000);
                      Delayus(4000000);
                      SetScreen(0);	      //��ʾ������                    
                      break;
                }   
		}    
		if(USART2_RX==0x51)
		{	   
			USART2->DR=0x54;                   //������ʼ��T
			while((USART2->SR&0X40)==0x00) {}; //�ȴ����ͽ���
					
			for(i=t*15;i<(t*15+15);i++)
			{
				USART2_printf("%d   ",Fre_Buffer[i]);               //����Ƶ��
				USART2_printf("%d    ",Impandence_Buffer[i]);       //�����迹
				USART2_printf("%f   \r\n",Angle[i]);		        //������λ		
			}
			t++;
			USART2->DR=0x55;                     //���ͽ�������U          
			while((USART2->SR&0X40)==0x00){};   //�ȴ����ͽ���
			USART2->DR=0x55;
			while((USART2->SR&0X40)==0x00){};   //�ȴ����ͽ���
            USART2_RX=0;
            SetProgressValue(0,24,t*100/68);
            sprintf((char*)buf,"%d",t*100/68);
            SetTextValue(0,25,buf); 	  
		}		
	} 
    Time_100Ms=0;
	ShowControl(0,28,1);       	//��ʾ���水ť	
    TIM_Cmd(TIM2, DISABLE);     //�رն�ʱ��2
}

/**********************************************************************
��������:void Send_Data_PC()
��    ��: �������ݵ���λ��
˵    ��:
��ڲ���: ��
�� �� ֵ: ��
***********************************************************************/
void Send_Data_PC()
{ 
    u8 Check=0;
	unsigned char buf[] = {0};
    u16 i;
	
    USART2_printf("%c",'Z');        //������ʼ��Z
	USART2->DR=0x0f;                //�����ұ߱�־
	while((USART2->SR&0X40)==0){};  
       
	sprintf((char*)buf,"%-10.1f",(double)Fre_Min);	//г��Ƶ��
	USART2_printf("%S",buf);    //DGUT//
    USART2_printf("%c",'V');    //ÿ�����ֵķָ���
    Check=buf[0];

	sprintf((char*)buf,"%-8.2f",(double)XZ_Impandence/1000 * 1.14651);//��̬����
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;
    USART2_printf("%c",'V');    //ÿ�����ֵķָ���

	sprintf((char*)buf,"%-10.1f",(double)Fre_F1);//�빦�ʵ�F1
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //ÿ�����ֵķָ���

	sprintf((char*)buf,"%-8.1f",(double)Fre_Max);//��г��Ƶ��
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //ÿ�����ֵķָ���
	
	sprintf((char*)buf,"%-10.2f",(double)YZ_Impandence/1000000*0.59896); //��г���迹
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //ÿ�����ֵķָ���
	
	sprintf((char*)buf,"%-10.1f",(double)Fre_F2); //�빦�ʵ�F2
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //ÿ�����ֵķָ���
	
	sprintf((char*)buf,"%-10.3f",(double)Qm * 1.029);//Ʒ������
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //ÿ�����ֵķָ���
	
	sprintf((char*)buf,"%-10.4f",(double)Keff * 0.97); //keff	
	USART2_printf("%S ",buf);    //DGUT// 
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //ÿ�����ֵķָ���

	sprintf((char*)buf,"%-10.3f",(double)fd);
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //ÿ�����ֵķָ���
	 
	 
	sprintf((char*)buf,"%-8.4f",CT*10000000000);//���ɵ���
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //ÿ�����ֵķָ���

	sprintf((char*)buf,"%-10.4f",(double)C1);     //��̬����
	USART2_printf("%S ",buf);    //DGUT// 
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //ÿ�����ֵķָ���

	sprintf((char*)buf,"%-10.4f",(double)C0);//��̬����
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //ÿ�����ֵķָ���
	 
	sprintf((char*)buf,"%-10.3f",(double)L1 * 1.45396); //��̬���
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');                //ÿ�����ֵķָ���
    USART_SendData(USART2,'W');             //һ֡�ķ��ͽ����� 
    USART_SendData(USART2,Check);           //У����    
    Delayus(1000);
      
    for(i=0;i<1000;i++)
    {
        USART2_printf("%c",'Z');                   //������ʼ��Z
        USART_SendData(USART2,0xf0);               //������߱�־λ

        USART2_printf("%ld",Fre_Buffer[i]);
        USART2_printf("%c",'V');                //ÿ�����ֵķָ���
        sprintf((char*)buf,"%-10.1f",(double)Fre_Buffer[i]);	
        Check=buf[0];

        USART2_printf("%ld",Impandence_Buffer[i]);
        USART2_printf("%c",'V');                //ÿ�����ֵķָ���
        sprintf((char*)buf,"%-10.1f",(double)Impandence_Buffer[i]);	
        Check=Check&buf[0];
        
        USART2_printf("%f",Angle[i]);
        USART2_printf("%c",'V');                //ÿ�����ֵķָ���
        sprintf((char*)buf,"%-10.1f",(double)Angle[i]);	
        Check=Check&buf[0];
       
        
        USART2_printf("%c",'W');            //һ֡�ķ��ͽ�����
        USART_SendData(USART2,Check);      //У����  
    }
}

