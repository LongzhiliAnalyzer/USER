/*********************************************************************
*                Copyright (C), 2015-2016, Supersonics. Co., Ltd.
*                        阻抗分析仪驱动程序
*
*                          硬件平台: xxx
*                          主 芯 片: STM32F103
*                          项 目 组: xxx
**********************************************************************
*文件名: control.c
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

#include "control.h"
#include "CtrFile.h"

#include "cmd_queue.h"
#include "AD9833.h"
#include <math.h>
#include <stdio.h>

#include "stdlib.h"
#include "ctype.h"

#include<string.h>
#include <stdarg.h>

// 小板新增部分

#include "delay.h"
#define CH375HF_NO_CODE
#include "CH375HFM.H"
#include "CH375DRV.H"
#include "stm32f10x.h"
#include "sys.h"

//struct znFAT_Init_Args Init_Args; //初始化TF卡参数集合
//struct FileInfo fileinfo; //文件信息集合
//struct DateTime dt; 
u8 times=0;
//u32 file_name=0; already defined in ctrlfile.c
UINT8 File_Name[20]="0";     //{"/youchao/",file_name,".txt"};
u32 t=0;
char SaveErrorFlag = 0;
//----小板新增部分 end----

u8 USART2_RX=0;

u32 Current_Fre=0;	//当前频率
u16 Current_V = 0;	//当前采集的电压值
u16 Current_A = 0;	//当前采集的电流值
u32 Impandence_Value = 0;//阻抗值

u32 Impandence_Value_Buffer=1500;

u32 FreGain = 5.0;                               /*扫频步进的变量*/

u16 ImpandenceStatus = Res01;                    /*阻抗切换时，进行状态选择*/
float SampleRes=5.0;                             /*阻抗标记*/
u32 Mid_Fre = 0;			                           /*定义并初始化谐振频率点*/
float Current_ARes =0.0;                         /*电流标记*/

s32 angle_P = 0;


u32 Impandence_Buffer[1024] = {0};               /*定义一个大的数组，用于保存扫频过程中得到的阻抗值*/
u16 Impandence_Buffer2[1024] = {0};              /*定义一个大的数组，用于保存对数转换后的阻抗值*/

u16 Current_V_Buffer[1024] = {0};
u16 Current_A_Buffer[1024] = {0};

u16 chart1[1024]={0};
u16 chart2u[1024]={0};
s16 chart2s[1024]={0};

u32 chart1_xaxis_min=0;
u32 chart1_xaxis_max=0;
u32 chart1_yaxis_min=0;
u32 chart1_yaxis_max=0;

u32 chart2_xaxis_max=0;
u32 chart2_xaxis_min=0;
u32 chart2_yaxis_max=0;
u32 chart2_yaxis_min=0;

u16 Impandence_Log10[1024] = {0};
u16 V_A_MAX=0;
u16 V_A_MIN=0;
s16 Angle_Buffer[1024] = {0};                    /*定义一个大的数组，用于保存扫频过程中得到的相位值（未经转换）*/
float Angle[1024] = {0.0};                       /*定义一个大的数组，用于保存扫频过程中得到的相位值*/
u32 Fre_Buffer[1024] = {0};                      /*保存扫频过程中的频率点数*/

u16 Impandence_Buffer_Flag = 0;                  /*用于记录扫频的位置*/
u32 Impandence_Value_Max = 0;                    /*最大阻抗值*/
u32 Impandence_Value_Min = 0;                    /*最小阻抗值*/
u32 Impandence_G_Buffer[2048] = {0};             /*   */
u32 XJ_Temp = 0;                                 /*   */
u32 XZ_Impandence = 0;                           /*   */
u32 YJ_Temp = 0;                                 /*   */
u32 YZ_Impandence = 0;                           /*   */

u16 Impandence_Log10_Max = 0;                    /*对数转换以后的阻抗最大值*/
u16 Impandence_Log10_Min = 0;                    /*对数转换以后的阻抗最小值*/




u32 Fre_Max = 0;                                 /*反谐振阻抗*/
u32 Fre_Min = 0;                                 /*谐振阻抗*/
s16 Angle_Buffer0 = 0;

u16 MaxValue_Flag = 0;
u16 MinValue_Flag = 0;


double kkk = 0.0;
double CT = 0.0;                                 /*用于计算静态电容C0的转换因子*/

double Qm = 0.0;                                 /*定义并初始化机械品质因素Qm*/
double L1 = 0.0;                                 /*定义并初始化动态电感L1*/
double C1 = 0.0;                                 /*定义并初始化动态电容C1*/
double C0 = 0.0;                                 /*定义并初始化静态电容C1*/
double Keff = 0.0;                               /*定义并初始化机电耦合系数*/
u32 Fre_F1 = 0;			       //定义并初始化半功率点F1
u32 Fre_F2 = 0;				     //定义并初始化半功率点F2
u32 fd = 0;

//u32 file_name=0;         //DGUT


//////

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

//////
void	mStopIfError( UINT8 iError )
{
	if ( iError == ERR_SUCCESS ) return;                        /* 操作成功 */
	printf( "Error: %02X\n", (UINT16)iError );                  /* 显示错误 */
	while ( 1 ) {

	}
}
/********************************
小板新增部分，用以实现保存功能的函数
*********************************/
void save_first()           
{
	int i = 0;
	char write_test0[50] = "0";	 
	char write_test1[50] = "0";
	char write_test2[50] = "0";
	char write_test3[50] = "0";
	char write_test4[50] = "0";
	
	char Head_String[] = "          阻抗分析仪\r\n";
	char Head_String0[] = "FS        R1        F1\r\n";	
	char Head_String1[] = "Fp        Zmax      F2\r\n";	
	char Head_String2[] = "Qm        keff      F2-F1\r\n";	
	char Head_String3[] = "CT        C1        C0\r\n";	
	char Head_String4[] = "L1\r\n";
	char Head_String5[] = "\r\nFre     Imp       Ang      U      I\r\n";
	
	char buf[50] = {0};
	char file[10] = "\\";
	
	sprintf((char*)buf, "%d", file_name);
	strcat(file, buf);
	strcat(file, ".TXT");
	
	sprintf((char*)buf,"%-10.1f",(double)Fre_Min);
	strcpy((char *)write_test0, (char *)buf);
	sprintf((char*)buf,"%-10.2f",(double)XZ_Impandence/1000 * 1.14651);
	strcat((char *)write_test0, (char *)buf);
	sprintf((char*)buf,"%-10.1f",(double)Fre_F1);//半功率点F1
	strcat((char *)write_test0, (char *)buf);
	strcat((char *)write_test0, "\r\n");
	
	sprintf((char*)buf,"%-10.1f",(double)Fre_Max);//反谐振频率
	strcpy((char *)write_test1, (char *)buf);
	sprintf((char*)buf,"%-10.2f",(double)YZ_Impandence/1000000*0.59896); //反谐振阻抗
	strcat((char *)write_test1, (char *)buf);
	sprintf((char*)buf,"%-10.1f",(double)Fre_F2); //半功率点F2
	strcat((char *)write_test1, (char *)buf);
	strcat((char *)write_test1, "\r\n");
	
	sprintf((char*)buf,"%-10.3f",(double)Qm * 1.029);//品质因素
	strcpy((char *)write_test2, (char *)buf);
	sprintf((char*)buf,"%-10.4f",(double)Keff * 0.97); //keff
	strcat((char *)write_test2, (char *)buf);
	sprintf((char*)buf,"%-10.3f",(double)fd);
	strcat((char *)write_test2, (char *)buf);
	strcat((char *)write_test2, "\r\n");
	
	sprintf((char*)buf,"%-10.4f",CT*10000000000);//自由电容
	strcpy((char *)write_test3, (char *)buf);
	sprintf((char*)buf,"%-10.4f",(double)C1);     //动态电容
	strcat((char *)write_test3, (char *)buf);
	sprintf((char*)buf,"%-10.4f",(double)C0);//静态电容
	strcat((char *)write_test3, (char *)buf);
	strcat((char *)write_test3, "\r\n");
	
	sprintf((char*)buf,"%-10.3f",(double)L1 * 1.45396); //动态电感
	strcpy((char *)write_test4, (char *)buf);
	strcat((char *)write_test4, "\r\n");
	
	strcpy((char *)mCmdParam.Create.mPathName, file);
	i = CH375FileCreate( );                               /* 新建文件并打开,如果文件已经存在则先删除后再新建 */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, Head_String);
	mCmdParam.ByteWrite.mByteCount = strlen(Head_String);
	i = CH375ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, Head_String0);
	mCmdParam.ByteWrite.mByteCount = strlen(Head_String0);
	i = CH375ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, write_test0);
	mCmdParam.ByteWrite.mByteCount = strlen(write_test0);
	i = CH375ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, Head_String1);
	mCmdParam.ByteWrite.mByteCount = strlen(Head_String1);
	i = CH375ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, write_test1);
	mCmdParam.ByteWrite.mByteCount = strlen(write_test1);
	i = CH375ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, Head_String2);
	mCmdParam.ByteWrite.mByteCount = strlen(Head_String2);
	i = CH375ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, write_test2);
	mCmdParam.ByteWrite.mByteCount = strlen(write_test2);
	i = CH375ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, Head_String3);
	mCmdParam.ByteWrite.mByteCount = strlen(Head_String3);
	i = CH375ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, write_test3);
	mCmdParam.ByteWrite.mByteCount = strlen(write_test3);
	i = CH375ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, Head_String4);
	mCmdParam.ByteWrite.mByteCount = strlen(Head_String4);
	i = CH375ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, write_test4);
	mCmdParam.ByteWrite.mByteCount = strlen(write_test4);
	i = CH375ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, Head_String5);
	mCmdParam.ByteWrite.mByteCount = strlen(Head_String5);
	i = CH375ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
} 



/*********************************************************************************
* 函数作用：保存左边数据
* 函数参数：无
* 函数返回值：无
**********************************************************************************/
void save_second()
{
	char buf1[15]="0";
	char buf2[15]="0";
	char buf3[15]="0";
	char buf4[15]="0";
	char buf5[15]="0";
	char buf[48]="0";
	int i = 0;
	
	sprintf((char*)buf1,"%-8d", Fre_Buffer[t]);
	sprintf((char*)buf2,"%-10d", Impandence_Buffer[t]);
	sprintf((char*)buf3,"%-8.2f", Angle[t]);
	sprintf((char*)buf4,"%-8d", Current_V_Buffer[t]);
	sprintf((char*)buf5,"%-8d", Current_A_Buffer[t]);
	
	strcpy((char *)buf,(char*)buf1);
	strcat((char *)buf,(char*)buf2);
	strcat((char *)buf,(char*)buf3);
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, buf);
	mCmdParam.ByteWrite.mByteCount = strlen(buf);
	i = CH375ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)buf,(char*)buf4);
	strcat((char *)buf,(char*)buf5);
	strcat((char *)buf, "\r\n");
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, buf);
	mCmdParam.ByteWrite.mByteCount = strlen(buf);
	i = CH375ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	t++;
	if(t % 20 == 0)
	{
		SetProgressValue(0,24,t*100/1000);
	}
}
/******************************************************************
* 函数作用：对数据进行中值滤波
* 函数参数：1、数据指针；2、滤波次数
* 函数返回值：滤波后的中间数值
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
* 函数作用：对数据进行均值滤波
* 函数参数：1、数据指针；2、滤波次数
* 函数返回值：滤波后的平均数值
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
* 函数作用：对数据进行中值平均滤波，对num个采集数据，去掉最大最小值，再求算术平均值
* 函数参数：1、数据指针；2、滤波次数
* 函数返回值：滤波后的平均数值
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
函数名称：void Phase_ValueFilter(u8 num)
功    能：滤波处理函数,对相位进行滤波处理
说    明：
入口参数：滤波阶数�?um(0、1、2、3...20)
返回值  ：无
********************************************************************/
void Phase_ValueFilter(u8 num)
{
	u16 i = 0;
	s16 Phase[30] = {0};

	TIM4->CCER &= (uint16_t)~((uint16_t)TIM_CCER_CC2E);	         //禁止二通道捕获
	TIM4->CCER |= (uint16_t)TIM_CCER_CC1E;				               //允许一通道捕获

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
函数名称：void ADC1_ValueFilter(u8 num) 功能是得到采样的电压、电流值
功    能：滤波处理函数 //采样并经过中值滤波而得到的电压和电流值；
说    明：
入口参数：滤波阶数�?um(0、1、2、3...20)
返回值  ：无
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
函数名称: void Send_Cmd(void)
功    能: 发送CT电容值的数据指令
说    明:
入口参数:
返 回 值: 无
***********************************************************************/
void Send_Cmd(void)
{
	u8 Parity = 0;
	u32 Capacitor = 0;
	Capacitor = CT*1000000;

	USART_SendData(USART1, 0x3C);  //发送同步字符
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
* 函数作用：阻抗切换判断函数
* 函数参数：无
* 函数返回值：无
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
函数名称: u16 Sweep(u32 Start_Fre,u32 End_Fre,u16 DAC_Value)
功    能: 扫频：最大电流反馈法获取系统谐振频率以及在此谐振频率下（实时锁相）的最大电压、最大电流、并换算得到阻抗值；
说    明:
入口参数:
返 回 值: 无
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

	queue_reset();					//清空串口接收缓冲区

	for(Current_Fre=Start_Fre; Current_Fre<=End_Fre;)
	{
		Fre_Buffer[i_flag++] = Current_Fre/1000;
		ad9833_out(Current_Fre/1000, 2);
		Delayus(10000);
	    ADC1_ValueFilter(20);					                                     //采样得到反馈的电压、电流值
		Phase_ValueFilter(20);
		Current_V_Buffer[(Current_Fre-Start_Fre)/FreGain]=Current_V;
		Current_A_Buffer[(Current_Fre-Start_Fre)/FreGain]=Current_A;

		Current_ARes = (float)Current_A/SampleRes;

    	Size = queue_find_cmd(cmd_buffer,1024);                            //从缓冲区中获取一条指令
		Message_Deal(Size);

		if(Stop_Control_Flag == 1)
		{
			return 1;
		}

		if(Current_Fre > (Start_Fre+Current_Buffer*(ProgressValue+1)))	   //进度条控制
		{
			ProgressValue++;
			if(ProgressValue<100)
			{
				SetProgressValue(0,24,ProgressValue);
				sprintf((char*)buf,"%d",ProgressValue);
				SetTextValue(0,25,buf);
			}
		}

		if(Current_A >= MaxAmp)	                                           //最大电流反馈法，扫频得到最大电流
		{
			MaxAmp = Current_A;
			Mid_Fre = Current_Fre/1000;
		}
		//计算阻抗
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

		Impandence_Buffer[Impandence_Buffer_Flag] = Impandence_Value_Buffer;                        //保存阻抗
		//Impandence_Log10[Impandence_Buffer_Flag] = log10((double)Impandence_Buffer[i_flag])*1000;
		Angle_Buffer[Impandence_Buffer_Flag] = ((angle_P)>>8)|(((angle_P)&0xFF)<<8);                //保存相位差

		Impandence_Log10[Impandence_Buffer_Flag]=Current_V;


		Angle[Impandence_Buffer_Flag] = (float)(angle_P-1820)/10;
		x = (Angle[Impandence_Buffer_Flag]/180)*PI;    //相位差转化为弧度
		//计算导纳

		//Impandence_G[Impandence_Buffer_Flag] = sqrt((((double)Impandence_Value_Buffer)*((double)Impandence_Value_Buffer))/(1+tan(x)*tan(x)));
		//XJ_Temp = Impandence_Value_Buffer - Impandence_G[Impandence_Buffer_Flag];
		XJ_Temp = Impandence_Value_Buffer - sqrt((((double)Impandence_Value_Buffer)*((double)Impandence_Value_Buffer))/(1+tan(x)*tan(x)));


		Impandence_G_Buffer[Impandence_Buffer_Flag] = Impandence_Value_Buffer + XJ_Temp;            //保存导纳

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

	ad9833_out(1000,2);           //1kHz输出
	Delayus(5000000);
	Delayus(5000000);
	ADC1_ValueFilter(20);					//采样得到反馈的电压、电流值

	kkk = (2*PI*1000*(((double)Current_V * 8.34348)/(double)(Current_A/3.420705/1000)));

	CT = (double)(1.0/kkk);       //计算1kHz下的自由电容

	GPIO_ResetBits(GPIOE,GPIO_Pin_15);
	GPIO_SetBits(GPIOE,GPIO_Pin_10);
	Delayus(5000000);

	/***********************************************************************************/
	for(i=0;i<10;i++)
	{   
	    Impandence_Log10[i]= Impandence_Log10[50];
		Angle_Buffer[i] = Angle_Buffer[10];
		Impandence_Buffer[i] = Impandence_Buffer[10];
	}

	Impandence_Value_Max = Impandence_Buffer[0];
	Impandence_Value = Impandence_Buffer[0];
	for(i_flag=0; i_flag<1000; i_flag++)
	{
		if(Impandence_Buffer[i_flag]<Impandence_Value)
		{
			Impandence_Value = Impandence_Buffer[i_flag];       //找到最小阻抗
			MinValue_Flag = i_flag;                             //最小阻抗对应的下标
		}
		if(Impandence_Buffer[i_flag]>Impandence_Value_Max)
		{
			Impandence_Value_Max = Impandence_Buffer[i_flag];   //找到最大阻抗
			MaxValue_Flag = i_flag;                             //最大阻抗对应的下标
		}
	}
	for(i_flag=0; i_flag<1000; i_flag++)
			{
				Impandence_Buffer2[i_flag] = ((u16)(log10((double)Impandence_Buffer[i_flag])*1000))>>8|((u16)(log10((double)Impandence_Buffer[i_flag])*1000)&0xff)<<8;
			}
	Impandence_Log10_Max = log10((double)Impandence_Value_Max)*1000;
	Impandence_Log10_Min = log10((double)Impandence_Value)*1000;
/*
	if(Display_Mode_Flag==2){
		
		memcpy(chart1,Current_V_Buffer,sizeof(u16)*1024);
		memcpy(chart2s,Current_A_Buffer,sizeof(s16)*1024);
		chart1_axis_max =  log10((double)Impandence_Value_Max)*1000;
		chart1_axis_min = log10((double)Impandence_Value)*1000;
		chart2_axis_max =  log10((double)Impandence_Value_Max)*1000;
		chart2_axis_min = log10((double)Impandence_Value)*1000;
	   }

	if(Display_Mode_Flag==1){
		
		chart1_axis_max =(u16)(Impandence_Value_Max>>16);
		chart1_axis_min = (u16)(Impandence_Value>>16);
		chart2_axis_max =  log10((double)Impandence_Value_Max)*1000;
		chart2_axis_min = log10((double)Impandence_Value)*1000;
		for(i_flag=0; i_flag<1000; i_flag++)
		{
			chart1[i_flag] = (u16)(Impandence_Buffer[i_flag]>>16)*0.45;
		}
		memcpy(chart2s,Angle_Buffer,sizeof(s16)*1024);

	}

	if(Display_Mode_Flag==0){
		
		memcpy(chart1,Impandence_Buffer2,sizeof(u16)*1024);
		memcpy(chart2s,Angle_Buffer,sizeof(s16)*1024);
		chart1_axis_max =  log10((double)Impandence_Value_Max)*1000;
		chart1_axis_min = log10((double)Impandence_Value)*1000;
		chart2_axis_max =  log10((double)Impandence_Value_Max)*1000;
		chart2_axis_min = log10((double)Impandence_Value)*1000;
	   }	*/
	x = (Angle[MinValue_Flag]/180)*PI;           //最小阻抗对应的相位差弧度值
	//计算最小阻抗
	Impandence_Value_Min = sqrt((((double)Impandence_Value)*((double)Impandence_Value))/(1+tan(x)*tan(x)));
	XJ_Temp = Impandence_Value - Impandence_Value_Min;
	XZ_Impandence = Impandence_Value + XJ_Temp;  //计算动态电阻

	y = (Angle[MaxValue_Flag]/180)*PI;           //最小阻抗对应的相位差弧度值
	//计算最大阻抗
	YJ_Temp = Impandence_Value_Max - sqrt((((double)Impandence_Value_Max)*((double)Impandence_Value_Max))/(1+tan(y)*tan(y)));
	YZ_Impandence = Impandence_Value_Max + YJ_Temp;//计算反谐振阻抗

	Fre_Max = Fre_Buffer[MaxValue_Flag];           //最大阻抗对应的频率
	Fre_Min = Fre_Buffer[MinValue_Flag];           //最小阻抗对应的频率
	//Impandence_Log10_Max = log10((double)Impandence_Value_Max)*1000;
	//Impandence_Log10_Min = log10((double)Impandence_Value)*1000;

	SetProgressValue(0,24,100);
	sprintf((char*)buf,"%d",100);
	SetTextValue(0,25,buf);
	ProgressValue = 0;
	Impandence_Buffer_Flag = 0;

	k_flag = MinValue_Flag;
	j_flag = MinValue_Flag;
	if((k_flag>0) || (j_flag>0))
	{
		while(Impandence_G_Buffer[k_flag] <= (2*XZ_Impandence) && Impandence_G_Buffer[j_flag] <= (2*XZ_Impandence))	  //求半功率点
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

	Fre_F1 = Fre_Buffer[k_flag + 1];    //半功率点F1
	Fre_F2 = Fre_Buffer[j_flag - 1];    //半功率点F2
	fd = (Fre_F2 - Fre_F1);             //F2-F1

	for(i_flag=0; i_flag<1000; i_flag++)
	{
//		//去掉干扰
//		if (Impandence_Buffer[i_flag + 1] < (Impandence_Buffer[i_flag] * 0.75)  && i_flag < 1000)
//		{
//			Impandence_Buffer[i_flag + 1] = Impandence_Buffer[i_flag] + 1;
//		}

		//将阻抗转化为对数值
//		Impandence_Buffer2[i_flag] = ((u16)(log10((double)Impandence_Buffer[i_flag])*1000))>>8|((u16)(log10((double)Impandence_Buffer[i_flag])*1000)&0xff)<<8;
	}

	Qm = (double)Fre_Min/fd;           //计算品质因素
	L1 = ((double)Impandence_Value/1000)/(2*PI*((double)fd/1000));    //计算动态电感
	C1 = (double)1*1000000000/(((double)4*PI*PI*(Fre_Min/1000)*(Fre_Min/1000)*(L1*1000))) * 0.65295;    //计算动态电容
	C0 = (CT*10000000000) - C1;       //计算动态电容
	//计算机电耦合系数
	Keff = sqrt(((double)(Fre_Max/1000)*(Fre_Max/1000)-(double)(Fre_Min/1000)*(Fre_Min/1000))/((Fre_Max/1000)*(Fre_Max/1000)));

	sprintf((char*)buf,"%-8.1f",(double)Fre_Max);
	SetTextValue(0,10,buf);     //反谐振频率

	
	sprintf((char*)buf,"%-10.2f",(double)YZ_Impandence/1000000*0.59896);
	SetTextValue(0,20,buf);     //反谐振阻抗

	
	sprintf((char*)buf,"%-10.1f",(double)Fre_Min);
	SetTextValue(0,9,buf);      //谐振频率
	
	sprintf((char*)buf,"%-10.1f",(double)Fre_F1);
	SetTextValue(0,14,buf);     //半功率点F1

	
	sprintf((char*)buf,"%-10.1f",(double)Fre_F2);
	SetTextValue(0,15,buf);     //半功率点F2

	
	sprintf((char*)buf,"%-10.3f",(double)Qm * 1.029);
	SetTextValue(0,12,buf);     //品质因素

	
	sprintf((char*)buf,"%-10.4f",(double)Keff * 0.97);
	SetTextValue(0,18,buf);     //keff

	
	sprintf((char*)buf,"%-10.4f",(double)C0);
	SetTextValue(0,16,buf);     //静态电容

	
	sprintf((char*)buf,"%-10.4f",(double)C1);
	SetTextValue(0,17,buf);     //动态电容

	
	sprintf((char*)buf,"%-10.3f",(double)L1 * 1.45396);
	SetTextValue(0,21,buf);     //动态电感

	
	sprintf((char*)buf,"%-10.3f",(double)fd);
	SetTextValue(0,19,buf);     //F2 - F1

	
	sprintf((char*)buf,"%-8.2f",(double)XZ_Impandence/1000 * 1.14651);
	SetTextValue(0,11,buf);     //动态电阻

	
	sprintf((char*)buf,"%-8.4f",CT*10000000000);
	SetTextValue(0,13,buf);     //自由电容


	ad9833_out(0,2);            //扫频结束禁止DDS输出
	AD9833_Init();
	

	return 0;
}

/*****************************************************************
与设定值比较
 *****************************************************************/

 int CampareandAlarm(double num1,double num2,double num3,double num4,double num5,double num6,double num7,double num8,double num9)
 {
   if((num1<xiezhen_minfreq)||(num1>xiezhen_maxfreq))	
   	{
		return 1;
   	}
	else if((num2<fanxiezhen_minfreq)||(num2>fanxiezhen_maxfreq))
   	{
		return 1;
   	}
	else if((num3<dongtai_minresis)||(num3>dongtai_maxresis))
   	{
		return 1;
   	}
	else if((num4<jingtai_mincapac)||(num4>jingtai_maxcapac))
   	{
		return 1;
   	}
	   if((num5<ziyou_mincapac)||(num5>ziyou_maxcapac))
   	{
		return 1;
   	}
	else if((num6<dongtai_mincapac)||(num6>dongtai_maxcapac)) 
   	{
		return 1;
   	}
	else if((num7<dongtai_minprod)||(num7>dongtai_maxprod))	
   	{
		return 1;
   	}
	else if((num8<fanxiezhen_minzukang)||(num8>fanxiezhen_maxzukang))
   	{
		return 1;
   	}
	else if((num9<pinzhiyinshu_min)||(num9>pinzhiyinshu_max))	
   	{
		return 1;
   	}
   	else
   		return 0;
  
 } 

 void chart(uint16 display_flag)
 {
		unsigned char Buff[] = {0};
		int i_flag=0;
		

		GraphChannelDataClear(0,23,0);
		GraphChannelDataClear(0,33,0);
		
		Delayus(100000);
		GraphChannelAdd(0,23,0,BLUE);			/*添加相位曲线通道	 */
		GraphChannelAdd(0,33,0,RED);			/*添加阻抗曲线通道	 */
		Delayus(10000);


		if(display_flag==0){
			memcpy(chart1,Impandence_Buffer2,sizeof(u16)*1024);
			memcpy(chart2s,Angle_Buffer,sizeof(s16)*1024);
			chart1_yaxis_max =  log10((double)Impandence_Value_Max)*1000;
			chart1_yaxis_min = log10((double)Impandence_Value)*1000;
			//chart2_yaxis_max =  log10((double)Impandence_Value_Max)*1000;
			//chart2_yaxis_min = log10((double)Impandence_Value)*1000;

			chart1_xaxis_max =  end_fre;
			chart1_xaxis_min = start_fre;
			chart2_xaxis_max = end_fre;
			chart2_xaxis_min = start_fre;
         	GraphSetViewport(0,23,0,33,0,5);   
			GraphSetViewport(0,33,0,33,chart1_yaxis_max-(185*(chart1_yaxis_max-chart1_yaxis_min)/180),18000/(chart1_yaxis_max-chart1_yaxis_min));		 //5-185
		 }
		if(display_flag==1){
			
			chart1_yaxis_max =(u16)(Impandence_Value_Max>>16);
			chart1_yaxis_min = (u16)(Impandence_Value>>16);
			//chart2_yaxis_max =  log10((double)Impandence_Value_Max)*1000;
			//chart2_yaxis_min = log10((double)Impandence_Value)*1000;
			chart1_xaxis_max =  end_fre;
			chart1_xaxis_min = start_fre;
			chart2_xaxis_max = end_fre;
			chart2_xaxis_min = start_fre;
			for(i_flag=0; i_flag<1000; i_flag++)
			{
				chart1[i_flag] = (u16)(Impandence_Buffer[i_flag]>>16)*0.45;
			}
			memcpy(chart2s,Angle_Buffer,sizeof(s16)*1024);
        	GraphSetViewport(0,23,0,33,0,5);    
			GraphSetViewport(0,33,0,33,0,1);		 //5-185
		 }
		if(display_flag==2){

			for(i_flag=0;i_flag<1024;i_flag++)
				{
				Current_V_Buffer[i_flag]*=0.01;
				Current_A_Buffer[i_flag]*=0.2;
			}
			memcpy(chart1,Current_V_Buffer,sizeof(u16)*1024);
			memcpy(chart2s,Current_A_Buffer,sizeof(s16)*1024);
			chart1_yaxis_max =0;
			chart1_yaxis_min =0;
		//	chart2_yaxis_max =  log10((double)Impandence_Value_Max)*1000;
		//	chart2_yaxis_min = log10((double)Impandence_Value)*1000;

			chart1_xaxis_max =  end_fre;
			chart1_xaxis_min = start_fre;
			chart2_xaxis_max = end_fre;
			chart2_xaxis_min = start_fre;
        	GraphSetViewport(0,23,0,33,0,1);    
			GraphSetViewport(0,33,0,33,0,1);		 
		 }
		if(display_flag==3){
        	GraphSetViewport(0,23,0,33,0,1);    
			GraphSetViewport(0,33,0,33,0,1);		
		 }
		sprintf((char*)Buff,"%-7.0f",(double)chart1_yaxis_min);
		SetTextValue(0,31,Buff);  
		sprintf((char*)Buff,"%-7.0f",(double)chart1_yaxis_max);
		SetTextValue(0,32,Buff);  
		sprintf((char*)Buff,"%-7.0f",(double)chart2_yaxis_min);
		SetTextValue(0,34,Buff);  
		sprintf((char*)Buff,"%-7.0f",(double)chart2_yaxis_max);
		SetTextValue(0,35,Buff);

		sprintf((char*)Buff,"%-7.0f",(double)chart1_xaxis_min);
		SetTextValue(0,37,Buff);  
		sprintf((char*)Buff,"%-7.0f",(double)chart1_xaxis_max);
		SetTextValue(0,36,Buff);  
		sprintf((char*)Buff,"%-7.0f",(double)chart2_xaxis_min);
		SetTextValue(0,38,Buff);  
		sprintf((char*)Buff,"%-7.0f",(double)chart2_xaxis_max);
		SetTextValue(0,30,Buff);
		  TIM_Cmd(TIM2, ENABLE);

	  		while (1)
	  		{	
				if ((Time_100Ms>0)&&(Time_100Ms<=1))
				{
				GraphChannelDataInsert(0,33,0,(u8*)chart1,2000);
				}
				else if ((Time_100Ms>1) && (Time_100Ms<3))
				{
				GraphChannelDataInsert(0,23,0,(u8*)chart2s,2000);
				}
				else if (Time_100Ms >= 3)
				{
				Time_100Ms = 0;
				break;
				}
			}
			TIM_Cmd(TIM2, DISABLE);
	 		
 }
/**********************************************************************
函数名称: void PhaseLock(void)
功    能: 扫频：通过频率连加、累减10次造成的相位差方向的改变，判定是否锁相成功
说    明:
入口参数:
返 回 值: 无
***********************************************************************/
 void PhaseLock(u32 Start_Fre,u32 End_Fre,u16 Voltage)
{
	AnimationPlayFrame(0,25,1); 

	if (Sweep(Start_Fre, End_Fre, Voltage) == 1)
	{   Stop_Button();
		Impandence_Buffer_Flag = 0;

		GPIO_ResetBits(GPIOE,GPIO_Pin_15);
    GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);		
	}
	else
	{
		ShowControl(0,3,1);     //启动按钮
		ShowControl(0,27,1);    //清屏按钮
		ShowControl(0,4,0);     //停止按钮
		ShowControl(0,28,1);    //保存数据按钮
		AnimationPlayFrame(0,25,0); 
		if(CampareandAlarm((double)Fre_Min,(double)Fre_Max,(double)XZ_Impandence/1000 * 1.14651,
			(double)C0,CT*10000000000,(double)C1,(double)L1 * 1.45396,(double)YZ_Impandence/1000000*0.59896,
			(double)Qm * 1.029)==1)
			
		{   SetBuzzer(60);
			AnimationPlayFrame(0,26,1); 
			SetTextValue( 0, 29,"NG");
		}else
		{   
			AnimationPlayFrame(0,26,2); 
			SetTextValue( 0, 29,"OK");
		}
		chart(Display_Mode_Flag);
	}
	AnimationPlayFrame(0,25,0); 
}

/**********************************************************************
函数名称: void USART2_printf (char *fmt,...)
功    能: 实现类似printf功能
说    明:
入口参数:
返 回 值: 无
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
函数名称:void Send_Space(u8 t)
功    能: 串口2发送空格，
说    明:
入口参数: t： 空格个数
返 回 值: 无
***********************************************************************/

void Send_Space(u8 t)
{
	u8 i=0;
	for(i=0;i<t;i++)
		USART2_printf(" ");
	
}
/**********************************************************************
函数名称:void USART2_IRQHandler()
功    能: 串口2接收中断
说    明:
入口参数:
返 回 值: 无
***********************************************************************/
void USART2_IRQHandler()
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  
	{

		USART2_RX =USART_ReceiveData(USART2);	//读取接收到的数据  
	}
}

/**********************************************************************
函数名称:void Send_Data_USB()
功    能: 发送数据到U盘的单片机
说    明:
入口参数: 无
返 回 值: 无
***********************************************************************/

void Send_Data_USB()
{  	
	u16 i=0;	
	ShowControl(0,28,1); 	
	
	delay_init();
	CH375_Init();
	CH375LibInit( );
	
	TIM_Cmd(TIM3, ENABLE); 
	while ( CH375DiskStatus < DISK_CONNECT ) {            /* 查询CH375中断并更新中断状态,等待U盘插入 */
		if ( CH375DiskConnect( ) == ERR_SUCCESS ) break;  /* 有设备连接则返回成功,CH375DiskConnect同时会更新全局变量CH375DiskStatus */
		if ( Time_100Ms_2 > 5 )
		{
			Time_100Ms_2 = 0;
			SetScreen(15);
			TIM_Cmd(TIM3, DISABLE);
			return;
		}
		delay_ms( 100 );
	}
	
	
	delay_ms(200);
	
	printf("disk init\n");
	for ( i = 0; i < 5; i ++ ) {                          /* 有的U盘总是返回未准备好,不过可以被忽略 */
		delay_ms( 100 );
		printf( "Ready ?\n" );
		if ( CH375DiskReady( ) == ERR_SUCCESS ) break;    /* 查询磁盘是否准备好 */
	}
	
	save_first();
	if (SaveErrorFlag == 1)
	{
		SaveErrorFlag = 0;
		SetScreen(2);			//显示保存失败
		return;
	}
	
	t = 0;
	while(t < 1000)
	{
		save_second();
		if (SaveErrorFlag == 1)
		{
			SaveErrorFlag = 0;
			SetScreen(2);			//显示保存失败
			return;
		}
	}
	
	mCmdParam.Close.mUpdateLen = 1;                       /* 不要自动计算文件长度,如果自动计算,那么该长度总是CH375vSectorSize的倍数 */
	i = CH375FileClose( );
	if ( i != ERR_SUCCESS )
	{
		SetScreen(2);			//显示保存失败
		return;
	}	
	
	t = 0;
	
	SetProgressValue(0,24,100);
	
	delay_ms(500);
	  
	SetScreen(3);	     //显时保存成功的界面
	Beep_On();         //开蜂鸣器
	Delayus(400000);
	Beep_Off();        //关蜂鸣器 
	t=0;      	
	Delayus(4000000);
	Delayus(4000000);
	Delayus(4000000);
	SetScreen(0);	     //显示主界面   


/**********************************
ch375读写测试程序
**********************************/
//	strcpy((char *)mCmdParam.Create.mPathName, "\\TITLE.TXT");   	//(文件名必须大写,且不能超过8个字符，后缀不能超过3个字符)

//	
//	i = CH375FileCreate( );                               /* 新建文件并打开,如果文件已经存在则先删除后再新建 */
//	mStopIfError( i );
//	
//	for (t = 0; t < 1000; t++)
//	{
//	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, "\r\n");
//	mCmdParam.ByteWrite.mByteCount = strlen("\r\n");
//	i = CH375ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
//	mStopIfError( i );
//	
//	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, "abcde");
//	mCmdParam.ByteWrite.mByteCount = strlen("abcde");                   /* 指定本次写入的字节数,单次读写的长度不能超过MAX_BYTE_IO */
//	i = CH375ByteWrite( );                                /* 以字节为单位向文件写入数据,单次读写的长度不能超过MAX_BYTE_IO */
//	mStopIfError( i );
//	}
//	
//	mCmdParam.Close.mUpdateLen = 1;                       /* 不要自动计算文件长度,如果自动计算,那么该长度总是CH375vSectorSize的倍数 */
//	i = CH375FileClose( );
//	mStopIfError( i );		
}

/**********************************************************************
函数名称:void Send_Data_PC()
功    能: 发送数据到上位机
说    明:
入口参数: 无
返 回 值: 无
***********************************************************************/
void Send_Data_PC()
{ 
    u8 Check=0;
	unsigned char buf[] = {0};
    u16 i;
	
    USART2_printf("%c",'Z');        //发送起始符Z
	USART2->DR=0x0f;                //发送右边标志
	while((USART2->SR&0X40)==0){};  
       
	sprintf((char*)buf,"%-10.1f",(double)Fre_Min);	//谐振频率
	USART2_printf("%S",buf);    //DGUT//
    USART2_printf("%c",'V');    //每个数字的分隔符
    Check=buf[0];

	sprintf((char*)buf,"%-8.2f",(double)XZ_Impandence/1000 * 1.14651);//动态电阻
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;
    USART2_printf("%c",'V');    //每个数字的分隔符

	sprintf((char*)buf,"%-10.1f",(double)Fre_F1);//半功率点F1
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //每个数字的分隔符

	sprintf((char*)buf,"%-8.1f",(double)Fre_Max);//反谐振频率
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //每个数字的分隔符
	
	sprintf((char*)buf,"%-10.2f",(double)YZ_Impandence/1000000*0.59896); //反谐振阻抗
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //每个数字的分隔符
	
	sprintf((char*)buf,"%-10.1f",(double)Fre_F2); //半功率点F2
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //每个数字的分隔符
	
	sprintf((char*)buf,"%-10.3f",(double)Qm * 1.029);//品质因素
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //每个数字的分隔符
	
	sprintf((char*)buf,"%-10.4f",(double)Keff * 0.97); //keff	
	USART2_printf("%S ",buf);    //DGUT// 
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //每个数字的分隔符

	sprintf((char*)buf,"%-10.3f",(double)fd);
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //每个数字的分隔符
	 
	 
	sprintf((char*)buf,"%-8.4f",CT*10000000000);//自由电容
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //每个数字的分隔符

	sprintf((char*)buf,"%-10.4f",(double)C1);     //动态电容
	USART2_printf("%S ",buf);    //DGUT// 
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //每个数字的分隔符

	sprintf((char*)buf,"%-10.4f",(double)C0);//静态电容
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');    //每个数字的分隔符
	 
	sprintf((char*)buf,"%-10.3f",(double)L1 * 1.45396); //动态电感
	USART2_printf("%S ",buf);    //DGUT//
    Check=buf[0]&Check;	
    USART2_printf("%c",'V');                //每个数字的分隔符
    USART_SendData(USART2,'W');             //一帧的发送结束符 
    USART_SendData(USART2,Check);           //校验码    
    Delayus(1000);
      
    for(i=0;i<1000;i++)
    {
        USART2_printf("%c",'Z');                   //发送起始符Z
        USART_SendData(USART2,0xf0);               //发送左边标志位

        USART2_printf("%ld",Fre_Buffer[i]);
        USART2_printf("%c",'V');                //每个数字的分隔符
        sprintf((char*)buf,"%-10.1f",(double)Fre_Buffer[i]);	
        Check=buf[0];

        USART2_printf("%ld",Impandence_Buffer[i]);
        USART2_printf("%c",'V');                //每个数字的分隔符
        sprintf((char*)buf,"%-10.1f",(double)Impandence_Buffer[i]);	
        Check=Check&buf[0];
        
        USART2_printf("%f",Angle[i]);
        USART2_printf("%c",'V');                //每个数字的分隔符
        sprintf((char*)buf,"%-10.1f",(double)Angle[i]);	
        Check=Check&buf[0];
       
        
        USART2_printf("%c",'W');            //一帧的发送结束符
        USART_SendData(USART2,Check);      //校验码  
    }
}

