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
#include "cmd_queue.h"
#include "AD9833.h"
#include <math.h>
#include <stdio.h>

#include "stdlib.h"
#include "ctype.h"

#include<string.h>
#include <stdarg.h>

// 小板新增部分
#include "znfat.h"
#include "delay.h"

struct znFAT_Init_Args Init_Args; //初始化TF卡参数集合
struct FileInfo fileinfo; //文件信息集合
struct DateTime dt; 
u8 times=0;
//u32 file_name=0; already defined in ctrlfile.c
INT8 File_Name[20]="0";     //{"/youchao/",file_name,".txt"};
u32 t=0;
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
u16 Impandence_Log10[1024] = {0};
s16 Angle_Buffer[1024] = {0};                    /*定义一个大的数组，用于保存扫频过程中得到的相位值（未经转换）*/
float Angle[1024] = {0.0};                       /*定义一个大的数组，用于保存扫频过程中得到的相位值*/
u32 Fre_Buffer[1024] = {0};                      /*保存扫频过程中的频率点数*/
u16 Impandence_Buffer_Flag = 0;                  /*用于记录扫频的位置*/
u32 Impandence_Value_Max = 0;                    /*最大阻抗值*/
u32 Impandence_Value_Min = 0;                    /*最小阻抗值*/
u32 Impandence_G[2048] = {0};                    /*   */
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

/********************************
小板新增部分，用以实现保存功能的函数
*********************************/
void save_first()           
{
	
	char buf[50] = {0};
	char buf2[15] = {0};
	char sz[10] = {0};
	unsigned char buf3[] = {0};
	u32 len=0;
	u32 i = 0;
	u16 jj = 0;
	u8 a=0,b=0,temp=0,temp2=0,res=0;
	u8 name_head[]="/youchao/";             //创建名为youchao为文件
	UINT8 ENTER[]="\r\n";
	//
	UINT8 write_data[20] = "0";
	//
	UINT8 write_test[50] = "0";	 
	UINT8 write_test1[50] = "0";
	UINT8 write_test2[50] = "0";
	UINT8 write_test3[50] = "0";
	UINT8 write_test4[50] = "0";
	UINT8 write_test5[] = "Fre    Imp		Ang\r\n";
	UINT8 Head_String[] = 
"           阻抗分析仪 拉拉         \r\n \
FS          R1        F1     	\r\n";	
	UINT8 Head_String1[] = "Fp          Zmax       F2     	\r\n ";	
	UINT8 Head_String2[] = "Qm          keff       F2-F1     	\r\n ";	
	UINT8 Head_String3[] = "CT          C1         C0     	\r\n ";	
	UINT8 Head_String4[] = "L1 \r\n";	
 	 
	
	for(a=0;a<20;a++)
		File_Name[a]=0;
	if(0==znFAT_Device_Init())		    //存储设备初始化		
       TIM_Cmd(TIM3, ENABLE);          //打开定时器
	znFAT_Select_Device(0,&Init_Args);  //选择设备0，也就是我的U盘
		
	res=znFAT_Init();                   //文件系统初始化
	
//	dt.date.year=2017;

//dt.date.month=11;

//dt.date.day=25;

//dt.time.hour=11;

//dt.time.min=14;

//dt.time.sec=36;
	if(!znFAT_Create_Dir("/youchao/",&dt))	
	{ //printf("creat suc\r\n"); 
		}		//创建文件夹
	else	{ //printf("dir has exist.\r\n");
		}
	znFAT_Flush_FS();				//刷新U盘


	for(a=0;a<sizeof(name_head);a++)	//将“name_head"赋给“file_Name“
	{
		File_Name[a]=name_head[a];			
	}
	//printf("translate\r\n");
	b=0;
	
	if(Time_100Ms_2>100)                      //定时时间10秒，保存错误的话将回到初始状态
	{
		Time_100Ms_2=0;
		TIM3->CNT=0;
		t=0;
		times=0;
		znFAT_Close_File(&fileinfo);
			
		TIM_Cmd(TIM3, DISABLE);          //关闭定时器3
		//printf("trans disconnected\r\n");
		return;
	}
	
	sprintf((char*)sz, "%d", file_name);
	
	for(a=0;a<sizeof(sz);a++)	//将"file_name"赋给“File_Name“
	{
		File_Name[9+a]=sz[a];			
	}
	Fre_Min = 10000;
	sprintf((char*)buf,"%-10.1f",(double)Fre_Min);
	for(a=0;a<sizeof(buf);a++)	//将参数转为字符存入数组
	{
		write_test[a]=buf[a];			
	}
	temp = sizeof(buf);
	sprintf((char*)buf,"%-8.2f",(double)XZ_Impandence/1000 * 1.14651);
	for(a=0;a<sizeof(buf);a++)	//将参数转为字符存入数组
	{
		write_test[a+temp]=buf[a];			
	}
	temp2 = sizeof(buf);
	sprintf((char*)buf,"%-10.1f",(double)Fre_F1);//半功率点F1
	for(a=0;a<sizeof(buf);a++)	//将参数转为字符存入数组
	{
		write_test[a+temp+temp2]=buf[a];			
	}
	
	sprintf((char*)buf,"%-8.1f",(double)Fre_Max);//反谐振频率
	write_test1[0] = buf[0];
	sprintf((char*)buf,"%-10.2f",(double)YZ_Impandence/1000000*0.59896); //反谐振阻抗
	write_test1[1] = buf[0];
	sprintf((char*)buf,"%-10.1f",(double)Fre_F2); //半功率点F2
	write_test1[2] = buf[0];
	
	sprintf((char*)buf,"%-10.3f",(double)Qm * 1.029);//品质因素
	write_test2[0] = buf[0];
	sprintf((char*)buf,"%-10.4f",(double)Keff * 0.97); //keff
	write_test2[1] = buf[0];
	sprintf((char*)buf,"%-10.3f",(double)fd);
	write_test2[2] = buf[0];
	
	sprintf((char*)buf,"%-8.4f",CT*10000000000);//自由电容
	write_test3[0] = buf[0];
	sprintf((char*)buf,"%-10.4f",(double)C1);     //动态电容
	write_test3[1] = buf[0];
	sprintf((char*)buf,"%-10.4f",(double)C0);//静态电容
	write_test3[2] = buf[0];
	
	sprintf((char*)buf,"%-10.3f",(double)L1 * 1.45396); //动态电感
	write_test4[0] = buf[0];
	
	
	TIM_Cmd(TIM3, DISABLE);          //关闭定时器3   
	for(a=0;File_Name[a]!=0;a++);
	File_Name[a++]=0x2e;                        //这里表示在文件名中后缀加上.txt
	File_Name[a++]=0x74;
	File_Name[a++]=0x78;
	File_Name[a++]=0x74;
	
	//printf("translate ok\r\n");
	//printf("%s\r\n",File_Name);
	if(!znFAT_Create_File(&fileinfo,File_Name,&dt))			//创建txt文件
	{
		//printf("creat file  suc\r\n");
		znFAT_Flush_FS();						//刷新U盘
		delay_ms(10);
//		res=znFAT_WriteData(&fileinfo,sizeof(Head_String),Head_String); 	//写入数据
//		if(!res)		printf("fail to write data.\n");
//		res=znFAT_WriteData(&fileinfo,50,write_test); 	                    //写入数据
//		res=znFAT_WriteData(&fileinfo,sizeof(ENTER),ENTER); 	            //换行
//		if(!res)		printf("fail to write data.\n");	

//		res=znFAT_WriteData(&fileinfo,sizeof(Head_String1),Head_String1); 	//写入数据
//		if(!res)		printf("fail to write data.\n");
//		res=znFAT_WriteData(&fileinfo,50,write_test1); 	                    //写入数据
//		res=znFAT_WriteData(&fileinfo,sizeof(ENTER),ENTER); 	            //换行
//		if(!res)		printf("fail to write data.\n");	

//		res=znFAT_WriteData(&fileinfo,sizeof(Head_String2),Head_String2); 	//写入数据
//		if(!res)		printf("fail to write data.\n");
//		res=znFAT_WriteData(&fileinfo,50,write_test2);                  	//写入数据
//		res=znFAT_WriteData(&fileinfo,sizeof(ENTER),ENTER); 	            //换行
//		if(!res)		printf("fail to write data.\n");	
//		
//		res=znFAT_WriteData(&fileinfo,sizeof(Head_String3),Head_String3); 	//写入数据
//		if(!res)		printf("fail to write data.\n");
//		res=znFAT_WriteData(&fileinfo,50,write_test3); 	                    //写入数据
//		res=znFAT_WriteData(&fileinfo,sizeof(ENTER),ENTER); 	            //换行
//		if(!res)		printf("fail to write data.\n");	
//		
//		res=znFAT_WriteData(&fileinfo,sizeof(Head_String4),Head_String4); 	//写入数据
//		if(!res)		printf("fail to write data.\n");
//		res=znFAT_WriteData(&fileinfo,50,write_test4); 	                    //写入数据
//		res=znFAT_WriteData(&fileinfo,sizeof(ENTER),ENTER); 	            //换行
//		if(!res)		printf("fail to write data.\n");	

//		res=znFAT_WriteData(&fileinfo,sizeof(write_test5),write_test5); 	//写入数据
//		if(!res)		printf("fail to write data.\n");
					
		
		
		
		
			//
		//znFAT_Close_File(&fileinfo);
		//delay_ms(50);
		//znFAT_Flush_FS();						//刷新U盘
//		delay_ms(50);
		//for(jj = 0; jj < 2; jj++)

		jj=0;
		delay_ms(50);
		for (i = 0; i < 1000 ;i++)
		{	
			sprintf((char*)buf2,"%-10.1f",(double)Impandence_Buffer2[i]);
			for(a=0;a<sizeof(buf2);a++)	//将参数转为字符存入数组
			{
				write_data[a]=buf2[a];			
			}
			strcat((char *)write_data, "\r\n");
			delay_ms(50);
			res=znFAT_WriteData(&fileinfo,sizeof(write_data),(UINT8 *)write_data); 	                    //写入数据
			//i=i+10;
			if(!res)	
			//	i=i+10;//	printf("fail to write data.\n");
			SetProgressValue(0,24, i*0.1);
//				sprintf((char*)buf3,"%d",i*0.1);
//				SetTextValue(0,25,buf3);
			Delayus(4000000);
		}	
		//znFAT_Close_File(&fileinfo);									
		delay_ms(50);
		//znFAT_Open_File(&fileinfo,File_Name,0,1);
	
		//znFAT_Close_File(&fileinfo);
		
		delay_ms(50);
		znFAT_Flush_FS();	//刷新U盘
		delay_ms(50);
		
//		znFAT_Open_File(&fileinfo,File_Name,0,1);
		
		delay_ms(50);
		delay_ms(50);

		Beep_On();         //开蜂鸣器
	Delayus(400000);
	Beep_Off();        //关蜂鸣器 
		
		
		
		znFAT_Close_File(&fileinfo);					                    //关闭文件			
	//	printf("save_first ok\r\n");
	}
	else		                        //如果存在了就打开
	{
		//printf("the file has existed");
	}	

	

		
		//
	times=1;
	 
	t=0;                               //发送0x51表示第一次的数据已经接受完毕
		
} 

void itoa (int n,char s[])
{
int i,j,sign;
if((sign=n)<0)//????
n=-n;//?n????
i=0;
do{
       s[i++]=n%10+'0';//??????
}
while ((n/=10)>0);//?????
if(sign<0)
s[i++]='-';
s[i]='\0';
for(j=i;j>=0;j--)//?????????,???????
       printf("%c",s[j]);
} 




/*********************************************************************************
* 函数作用：保存左边数据
* 函数参数：无
* 函数返回值：无
**********************************************************************************/
void save_second()
{
UINT8 buf1[15]="123";
UINT8 buf2[15]="123";
UINT8 buf3[15]="123\r\n";
UINT8 buf4[48]={0};
	unsigned char buf[] = {0};
	u8 res=0;
	UINT32 len=0;
	u16 i=0;
	int j = 0;
    u8 a=0;
	UINT8 write_data[20] = "0";


	znFAT_Flush_FS();
	len = 48;
	
//	     sprintf((char*)buf2,"%-10.1f",(double)Impandence_Buffer2[t]);
//				for(a=0;a<sizeof(buf2);a++)	//将参数转为字符存入数组
//				{
//					write_data[a]=buf2[a];			
//				}
//				strcat((char *)write_data, "\r\n");
	
//	
//		buf[j] = Fre_Buffer[t];
//		j++;	
//		
//		buf[j] = Impandence_Buffer[t];
//		j++;	
//	
//		buf[j] = Angle[t];
//		j++;	
	

		
	delay_ms(50);
	if(!znFAT_Open_File(&fileinfo,File_Name,0,1))			//打开指定的txt文件
	{
		//for(i=10*t;i<(10+10*t);i++)
		{
			
//			USART2_printf("%d   ",Fre_Buffer[i]);               //发送频率
//				USART2_printf("%d    ",Impandence_Buffer[i]);       //发送阻抗
//				USART2_printf("%f   \r\n",Angle[i]);		        //发送相位		
//			sprintf((char*)buf1,"%10d",Fre_Buffer[i]);
//			sprintf((char*)buf2,"%10d",Impandence_Buffer[i]);
//			sprintf((char*)buf3,"%10d",(u32)Angle[i]);
			
			sprintf((char*)buf1,"%-10d",Fre_Buffer[t]);
			sprintf((char*)buf2,"%-10d",Impandence_Buffer[t]);
			sprintf((char*)buf3,"%-10d",( int )(Angle[t]*10));
			
			//itoa(i, buf1);
			
			strcat((char *)buf4,(char*)buf1);
			strcat((char *)buf4,(char*)buf2);
			strcat((char *)buf4,(char*)buf3);
			strcat((char *)buf4, "\r\n");
			
//			res=znFAT_WriteData(&fileinfo,sizeof(buf1),buf1); 	//写入数据
//			delay_ms(20);
//			res=znFAT_WriteData(&fileinfo,sizeof(buf2),buf2); 	//写入数据
//			delay_ms(20);
			res=znFAT_WriteData(&fileinfo,sizeof(buf4),buf4); 	//写入数据
			delay_ms(20);
//			res=znFAT_WriteData(&fileinfo,sizeof(ENTER),ENTER); 	//写入数据
//			delay_ms(20);
		}
		
			Beep_On();         //开蜂鸣器
			Delayus(400000);
			Beep_Off();        //关蜂鸣器 
//		res=znFAT_WriteData(&fileinfo,sizeof(ENTER),ENTER); 	            //换行
		//printf("%d\r\n",res);
		znFAT_Close_File(&fileinfo);
	}
	else	
		{
			
			//printf("open file fail\r\n");
	}
	
	
	
	t=t+10;
	delay_ms(50);
	Delayus(3000000);
	//Delayus(3000000);
	znFAT_Flush_FS();
	Delayus(3000000);
//	SetProgressValue(0,24,t*100/68);
//	sprintf((char*)buf,"%d",t*100/68);
//	SetTextValue(0,25,buf); 	
		
	
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
入口参数：滤波阶数num(0、1、2、3...20)
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
入口参数：滤波阶数num(0、1、2、3...20)
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
		Current_ARes = (float)Current_A/SampleRes;

    Size = queue_find_cmd(cmd_buffer,1024);                            //从缓冲区中获取一条指令
		Message_Deal(Size);

		if(Stop_Control_Flag == 1)
		{
			return 0;
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
		Impandence_Log10[Impandence_Buffer_Flag] = log10((double)Impandence_Buffer[i_flag])*1000;
		Angle_Buffer[Impandence_Buffer_Flag] = ((angle_P)>>8)|(((angle_P)&0xFF)<<8);                //保存相位差

		Angle[Impandence_Buffer_Flag] = (float)(angle_P-1820)/10;
		x = (Angle[Impandence_Buffer_Flag]/180)*PI;    //相位差转化为弧度
		//计算导纳
		Impandence_G[Impandence_Buffer_Flag] = sqrt((((double)Impandence_Value_Buffer)*((double)Impandence_Value_Buffer))/(1+tan(x)*tan(x)));
		XJ_Temp = Impandence_Value_Buffer - Impandence_G[Impandence_Buffer_Flag];
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
		Impandence_Buffer2[i_flag] = ((u16)(log10((double)Impandence_Buffer[i_flag])*1000))>>8|((u16)(log10((double)Impandence_Buffer[i_flag])*1000)&0xff)<<8;
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


	AnimationPlayFrame(0,2,0);  //工作状态置位

	ad9833_out(0,2);            //扫频结束禁止DDS输出
	AD9833_Init();
	

	return 1;
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
//	u16 t;
	if (Sweep(Start_Fre, End_Fre, Voltage) == 1)
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
		GraphSetViewport(0,23,0,33,0,5);    //相位曲线
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
		TIM_Cmd(TIM2, DISABLE);
	}
	else
	{
		Stop_Button();
		Impandence_Buffer_Flag = 0;

		GPIO_ResetBits(GPIOE,GPIO_Pin_15);
    GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14);
	}
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
	unsigned char buf[] = {0};
	u16 i=0;	
	ShowControl(0,28,1); 	
	
	save_first();				//格式还有问题
	
	while(t < 1000)
	{
		save_second();	//这个是还没存进去， 可能出现内存溢出或者访问越界或者堆栈溢出
		delay_ms(100);	
	}
	delay_ms(100);
	delay_ms(100);
	delay_ms(100);

	t = 0;
	
	SetProgressValue(0,24,100);
	sprintf((char*)buf,"%d",100);
	SetTextValue(0,25,buf);
	Delayus(4000000);
	  
	SetScreen(3);	     //显时保存成功的界面
	Beep_On();         //开蜂鸣器
	Delayus(400000);
	Beep_Off();        //关蜂鸣器 
	t=0;      	
	Delayus(4000000);
	Delayus(4000000);
	Delayus(4000000);
	SetScreen(0);	     //显示主界面   
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

