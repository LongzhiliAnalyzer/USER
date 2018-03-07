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

#include "stdlib.h"
#include "ctype.h"

#include<string.h>
#include <stdarg.h>

// Ð¡°åÐÂÔö²¿·Ö

#include "delay.h"
#define CH375HF_NO_CODE
#include "CH375HFM.H"
#include "CH375DRV.H"
#include "stm32f10x.h"
#include "sys.h"

//struct znFAT_Init_Args Init_Args; //³õÊ¼»¯TF¿¨²ÎÊý¼¯ºÏ
//struct FileInfo fileinfo; //ÎÄ¼þÐÅÏ¢¼¯ºÏ
//struct DateTime dt; 
u8 times=0;
//u32 file_name=0; already defined in ctrlfile.c
UINT8 File_Name[20]="0";     //{"/youchao/",file_name,".txt"};
u32 t=0;
char SaveErrorFlag = 0;
//----Ð¡°åÐÂÔö²¿·Ö end----

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


u32 Impandence_Buffer[1024] = {0};               /*¶¨ÒåÒ»¸ö´óµÄÊý×é£¬ÓÃÓÚ±£´æÉ¨Æµ¹ý³ÌÖÐµÃµ½µÄ×è¿¹Öµ*/
u16 Impandence_Buffer2[1024] = {0};              /*¶¨ÒåÒ»¸ö´óµÄÊý×é£¬ÓÃÓÚ±£´æ¶ÔÊý×ª»»ºóµÄ×è¿¹Öµ*/

u16 Current_V_Buffer[1024] = {0};
u16 Current_A_Buffer[1024] = {0};

u16 chart1[1024]={0};
u16 chart2u[1024]={0};
s16 chart2s[1024]={0};

u16 chart1_xaxis_min=0;
u16 chart1_xaxis_max=0;
u16 chart1_yaxis_min=0;
u16 chart1_yaxis_max=0;

u16 chart2_xaxis_max=0;
u16 chart2_xaxis_min=0;
u16 chart2_yaxis_max=0;
u16 chart2_yaxis_min=0;

u16 Impandence_Log10[1024] = {0};
u16 V_A_MAX=0;
u16 V_A_MIN=0;
s16 Angle_Buffer[1024] = {0};                    /*¶¨ÒåÒ»¸ö´óµÄÊý×é£¬ÓÃÓÚ±£´æÉ¨Æµ¹ý³ÌÖÐµÃµ½µÄÏàÎ»Öµ£¨Î´¾­×ª»»£©*/
float Angle[1024] = {0.0};                       /*¶¨ÒåÒ»¸ö´óµÄÊý×é£¬ÓÃÓÚ±£´æÉ¨Æµ¹ý³ÌÖÐµÃµ½µÄÏàÎ»Öµ*/
u32 Fre_Buffer[1024] = {0};                      /*±£´æÉ¨Æµ¹ý³ÌÖÐµÄÆµÂÊµãÊý*/

u16 Impandence_Buffer_Flag = 0;                  /*ÓÃÓÚ¼ÇÂ¼É¨ÆµµÄÎ»ÖÃ*/
u32 Impandence_Value_Max = 0;                    /*×î´ó×è¿¹Öµ*/
u32 Impandence_Value_Min = 0;                    /*×îÐ¡×è¿¹Öµ*/
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


//////

//////////////////////////////////////////////////////////////////
//¼ÓÈëÒÔÏÂ´úÂë,Ö§³Öprintfº¯Êý,¶ø²»ÐèÒªÑ¡Ôñuse MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//±ê×¼¿âÐèÒªµÄÖ§³Öº¯Êý                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//¶¨Òå_sys_exit()ÒÔ±ÜÃâÊ¹ÓÃ°ëÖ÷»úÄ£Ê½    
_sys_exit(int x) 
{ 
	x = x; 
} 
//ÖØ¶¨Òåfputcº¯Êý 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//Ñ­»··¢ËÍ,Ö±µ½·¢ËÍÍê±Ï   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

//////
void	mStopIfError( UINT8 iError )
{
	if ( iError == ERR_SUCCESS ) return;                        /* ²Ù×÷³É¹¦ */
	printf( "Error: %02X\n", (UINT16)iError );                  /* ÏÔÊ¾´íÎó */
	while ( 1 ) {

	}
}
/********************************
Ð¡°åÐÂÔö²¿·Ö£¬ÓÃÒÔÊµÏÖ±£´æ¹¦ÄÜµÄº¯Êý
*********************************/
void save_first()           
{
	int i = 0;
	char write_test0[50] = "0";	 
	char write_test1[50] = "0";
	char write_test2[50] = "0";
	char write_test3[50] = "0";
	char write_test4[50] = "0";
	
	char Head_String[] = "          ×è¿¹·ÖÎöÒÇ\r\n";
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
	sprintf((char*)buf,"%-10.1f",(double)Fre_F1);//°ë¹¦ÂÊµãF1
	strcat((char *)write_test0, (char *)buf);
	strcat((char *)write_test0, "\r\n");
	
	sprintf((char*)buf,"%-10.1f",(double)Fre_Max);//·´Ð³ÕñÆµÂÊ
	strcpy((char *)write_test1, (char *)buf);
	sprintf((char*)buf,"%-10.2f",(double)YZ_Impandence/1000000*0.59896); //·´Ð³Õñ×è¿¹
	strcat((char *)write_test1, (char *)buf);
	sprintf((char*)buf,"%-10.1f",(double)Fre_F2); //°ë¹¦ÂÊµãF2
	strcat((char *)write_test1, (char *)buf);
	strcat((char *)write_test1, "\r\n");
	
	sprintf((char*)buf,"%-10.3f",(double)Qm * 1.029);//Æ·ÖÊÒòËØ
	strcpy((char *)write_test2, (char *)buf);
	sprintf((char*)buf,"%-10.4f",(double)Keff * 0.97); //keff
	strcat((char *)write_test2, (char *)buf);
	sprintf((char*)buf,"%-10.3f",(double)fd);
	strcat((char *)write_test2, (char *)buf);
	strcat((char *)write_test2, "\r\n");
	
	sprintf((char*)buf,"%-10.4f",CT*10000000000);//×ÔÓÉµçÈÝ
	strcpy((char *)write_test3, (char *)buf);
	sprintf((char*)buf,"%-10.4f",(double)C1);     //¶¯Ì¬µçÈÝ
	strcat((char *)write_test3, (char *)buf);
	sprintf((char*)buf,"%-10.4f",(double)C0);//¾²Ì¬µçÈÝ
	strcat((char *)write_test3, (char *)buf);
	strcat((char *)write_test3, "\r\n");
	
	sprintf((char*)buf,"%-10.3f",(double)L1 * 1.45396); //¶¯Ì¬µç¸Ð
	strcpy((char *)write_test4, (char *)buf);
	strcat((char *)write_test4, "\r\n");
	
	strcpy((char *)mCmdParam.Create.mPathName, file);
	i = CH375FileCreate( );                               /* ÐÂ½¨ÎÄ¼þ²¢´ò¿ª,Èç¹ûÎÄ¼þÒÑ¾­´æÔÚÔòÏÈÉ¾³ýºóÔÙÐÂ½¨ */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, Head_String);
	mCmdParam.ByteWrite.mByteCount = strlen(Head_String);
	i = CH375ByteWrite( );                                /* ÒÔ×Ö½ÚÎªµ¥Î»ÏòÎÄ¼þÐ´ÈëÊý¾Ý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, Head_String0);
	mCmdParam.ByteWrite.mByteCount = strlen(Head_String0);
	i = CH375ByteWrite( );                                /* ÒÔ×Ö½ÚÎªµ¥Î»ÏòÎÄ¼þÐ´ÈëÊý¾Ý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, write_test0);
	mCmdParam.ByteWrite.mByteCount = strlen(write_test0);
	i = CH375ByteWrite( );                                /* ÒÔ×Ö½ÚÎªµ¥Î»ÏòÎÄ¼þÐ´ÈëÊý¾Ý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, Head_String1);
	mCmdParam.ByteWrite.mByteCount = strlen(Head_String1);
	i = CH375ByteWrite( );                                /* ÒÔ×Ö½ÚÎªµ¥Î»ÏòÎÄ¼þÐ´ÈëÊý¾Ý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, write_test1);
	mCmdParam.ByteWrite.mByteCount = strlen(write_test1);
	i = CH375ByteWrite( );                                /* ÒÔ×Ö½ÚÎªµ¥Î»ÏòÎÄ¼þÐ´ÈëÊý¾Ý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, Head_String2);
	mCmdParam.ByteWrite.mByteCount = strlen(Head_String2);
	i = CH375ByteWrite( );                                /* ÒÔ×Ö½ÚÎªµ¥Î»ÏòÎÄ¼þÐ´ÈëÊý¾Ý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, write_test2);
	mCmdParam.ByteWrite.mByteCount = strlen(write_test2);
	i = CH375ByteWrite( );                                /* ÒÔ×Ö½ÚÎªµ¥Î»ÏòÎÄ¼þÐ´ÈëÊý¾Ý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, Head_String3);
	mCmdParam.ByteWrite.mByteCount = strlen(Head_String3);
	i = CH375ByteWrite( );                                /* ÒÔ×Ö½ÚÎªµ¥Î»ÏòÎÄ¼þÐ´ÈëÊý¾Ý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, write_test3);
	mCmdParam.ByteWrite.mByteCount = strlen(write_test3);
	i = CH375ByteWrite( );                                /* ÒÔ×Ö½ÚÎªµ¥Î»ÏòÎÄ¼þÐ´ÈëÊý¾Ý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, Head_String4);
	mCmdParam.ByteWrite.mByteCount = strlen(Head_String4);
	i = CH375ByteWrite( );                                /* ÒÔ×Ö½ÚÎªµ¥Î»ÏòÎÄ¼þÐ´ÈëÊý¾Ý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, write_test4);
	mCmdParam.ByteWrite.mByteCount = strlen(write_test4);
	i = CH375ByteWrite( );                                /* ÒÔ×Ö½ÚÎªµ¥Î»ÏòÎÄ¼þÐ´ÈëÊý¾Ý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, Head_String5);
	mCmdParam.ByteWrite.mByteCount = strlen(Head_String5);
	i = CH375ByteWrite( );                                /* ÒÔ×Ö½ÚÎªµ¥Î»ÏòÎÄ¼þÐ´ÈëÊý¾Ý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
	if ( i != ERR_SUCCESS )
	{
		SaveErrorFlag = 1;
		return;
	}
	
} 



/*********************************************************************************
* º¯Êý×÷ÓÃ£º±£´æ×ó±ßÊý¾Ý
* º¯Êý²ÎÊý£ºÎÞ
* º¯Êý·µ»ØÖµ£ºÎÞ
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
	i = CH375ByteWrite( );                                /* ÒÔ×Ö½ÚÎªµ¥Î»ÏòÎÄ¼þÐ´ÈëÊý¾Ý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
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
	i = CH375ByteWrite( );                                /* ÒÔ×Ö½ÚÎªµ¥Î»ÏòÎÄ¼þÐ´ÈëÊý¾Ý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
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
		Current_V_Buffer[(Current_Fre-Start_Fre)/FreGain]=Current_V;
		Current_A_Buffer[(Current_Fre-Start_Fre)/FreGain]=Current_A;

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
		//Impandence_Log10[Impandence_Buffer_Flag] = log10((double)Impandence_Buffer[i_flag])*1000;
		Angle_Buffer[Impandence_Buffer_Flag] = ((angle_P)>>8)|(((angle_P)&0xFF)<<8);                //±£´æÏàÎ»²î

		Impandence_Log10[Impandence_Buffer_Flag]=Current_V;


		Angle[Impandence_Buffer_Flag] = (float)(angle_P-1820)/10;
		x = (Angle[Impandence_Buffer_Flag]/180)*PI;    //ÏàÎ»²î×ª»¯Îª»¡¶È
		//¼ÆËãµ¼ÄÉ

		//Impandence_G[Impandence_Buffer_Flag] = sqrt((((double)Impandence_Value_Buffer)*((double)Impandence_Value_Buffer))/(1+tan(x)*tan(x)));
		//XJ_Temp = Impandence_Value_Buffer - Impandence_G[Impandence_Buffer_Flag];
		XJ_Temp = Impandence_Value_Buffer - sqrt((((double)Impandence_Value_Buffer)*((double)Impandence_Value_Buffer))/(1+tan(x)*tan(x)));


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
			Impandence_Value = Impandence_Buffer[i_flag];       //ÕÒµ½×îÐ¡×è¿¹
			MinValue_Flag = i_flag;                             //×îÐ¡×è¿¹¶ÔÓ¦µÄÏÂ±ê
		}
		if(Impandence_Buffer[i_flag]>Impandence_Value_Max)
		{
			Impandence_Value_Max = Impandence_Buffer[i_flag];   //ÕÒµ½×î´ó×è¿¹
			MaxValue_Flag = i_flag;                             //×î´ó×è¿¹¶ÔÓ¦µÄÏÂ±ê
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
//		Impandence_Buffer2[i_flag] = ((u16)(log10((double)Impandence_Buffer[i_flag])*1000))>>8|((u16)(log10((double)Impandence_Buffer[i_flag])*1000)&0xff)<<8;
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


	ad9833_out(0,2);            //É¨Æµ½áÊø½ûÖ¹DDSÊä³ö
	AD9833_Init();
	

	return 0;
}

/*****************************************************************
ÓëÉè¶¨Öµ±È½Ï
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
		GraphChannelAdd(0,23,0,BLUE);			/*Ìí¼ÓÏàÎ»ÇúÏßÍ¨µÀ	 */
		GraphChannelAdd(0,33,0,RED);			/*Ìí¼Ó×è¿¹ÇúÏßÍ¨µÀ	 */
		Delayus(10000);


		if(display_flag==0){
			memcpy(chart1,Impandence_Buffer2,sizeof(u16)*1024);
			memcpy(chart2s,Angle_Buffer,sizeof(s16)*1024);
			chart1_yaxis_max =  log10((double)Impandence_Value_Max)*1000;
			chart1_yaxis_min = log10((double)Impandence_Value)*1000;
			chart2_yaxis_max =  log10((double)Impandence_Value_Max)*1000;
			chart2_yaxis_min = log10((double)Impandence_Value)*1000;

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
			chart2_yaxis_max =  log10((double)Impandence_Value_Max)*1000;
			chart2_yaxis_min = log10((double)Impandence_Value)*1000;
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
			chart1_yaxis_max =  log10((double)Impandence_Value_Max)*1000;
			chart1_yaxis_min = log10((double)Impandence_Value)*1000;
			chart2_yaxis_max =  log10((double)Impandence_Value_Max)*1000;
			chart2_yaxis_min = log10((double)Impandence_Value)*1000;
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
º¯ÊýÃû³Æ: void PhaseLock(void)
¹¦    ÄÜ: É¨Æµ£ºÍ¨¹ýÆµÂÊÁ¬¼Ó¡¢ÀÛ¼õ10´ÎÔì³ÉµÄÏàÎ»²î·½ÏòµÄ¸Ä±ä£¬ÅÐ¶¨ÊÇ·ñËøÏà³É¹¦
Ëµ    Ã÷:
Èë¿Ú²ÎÊý:
·µ »Ø Öµ: ÎÞ
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
		ShowControl(0,3,1);     //Æô¶¯°´Å¥
		ShowControl(0,27,1);    //ÇåÆÁ°´Å¥
		ShowControl(0,4,0);     //Í£Ö¹°´Å¥
		ShowControl(0,28,1);    //±£´æÊý¾Ý°´Å¥
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
	u16 i=0;	
	ShowControl(0,28,1); 	
	
	delay_init();
	CH375_Init();
	CH375LibInit( );
	
	TIM_Cmd(TIM3, ENABLE); 
	while ( CH375DiskStatus < DISK_CONNECT ) {            /* ²éÑ¯CH375ÖÐ¶Ï²¢¸üÐÂÖÐ¶Ï×´Ì¬,µÈ´ýUÅÌ²åÈë */
		if ( CH375DiskConnect( ) == ERR_SUCCESS ) break;  /* ÓÐÉè±¸Á¬½ÓÔò·µ»Ø³É¹¦,CH375DiskConnectÍ¬Ê±»á¸üÐÂÈ«¾Ö±äÁ¿CH375DiskStatus */
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
	for ( i = 0; i < 5; i ++ ) {                          /* ÓÐµÄUÅÌ×ÜÊÇ·µ»ØÎ´×¼±¸ºÃ,²»¹ý¿ÉÒÔ±»ºöÂÔ */
		delay_ms( 100 );
		printf( "Ready ?\n" );
		if ( CH375DiskReady( ) == ERR_SUCCESS ) break;    /* ²éÑ¯´ÅÅÌÊÇ·ñ×¼±¸ºÃ */
	}
	
	save_first();
	if (SaveErrorFlag == 1)
	{
		SaveErrorFlag = 0;
		SetScreen(2);			//ÏÔÊ¾±£´æÊ§°Ü
		return;
	}
	
	t = 0;
	while(t < 1000)
	{
		save_second();
		if (SaveErrorFlag == 1)
		{
			SaveErrorFlag = 0;
			SetScreen(2);			//ÏÔÊ¾±£´æÊ§°Ü
			return;
		}
	}
	
	mCmdParam.Close.mUpdateLen = 1;                       /* ²»Òª×Ô¶¯¼ÆËãÎÄ¼þ³¤¶È,Èç¹û×Ô¶¯¼ÆËã,ÄÇÃ´¸Ã³¤¶È×ÜÊÇCH375vSectorSizeµÄ±¶Êý */
	i = CH375FileClose( );
	if ( i != ERR_SUCCESS )
	{
		SetScreen(2);			//ÏÔÊ¾±£´æÊ§°Ü
		return;
	}	
	
	t = 0;
	
	SetProgressValue(0,24,100);
	
	delay_ms(500);
	  
	SetScreen(3);	     //ÏÔÊ±±£´æ³É¹¦µÄ½çÃæ
	Beep_On();         //¿ª·äÃùÆ÷
	Delayus(400000);
	Beep_Off();        //¹Ø·äÃùÆ÷ 
	t=0;      	
	Delayus(4000000);
	Delayus(4000000);
	Delayus(4000000);
	SetScreen(0);	     //ÏÔÊ¾Ö÷½çÃæ   


/**********************************
ch375¶ÁÐ´²âÊÔ³ÌÐò
**********************************/
//	strcpy((char *)mCmdParam.Create.mPathName, "\\TITLE.TXT");   	//(ÎÄ¼þÃû±ØÐë´óÐ´,ÇÒ²»ÄÜ³¬¹ý8¸ö×Ö·û£¬ºó×º²»ÄÜ³¬¹ý3¸ö×Ö·û)

//	
//	i = CH375FileCreate( );                               /* ÐÂ½¨ÎÄ¼þ²¢´ò¿ª,Èç¹ûÎÄ¼þÒÑ¾­´æÔÚÔòÏÈÉ¾³ýºóÔÙÐÂ½¨ */
//	mStopIfError( i );
//	
//	for (t = 0; t < 1000; t++)
//	{
//	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, "\r\n");
//	mCmdParam.ByteWrite.mByteCount = strlen("\r\n");
//	i = CH375ByteWrite( );                                /* ÒÔ×Ö½ÚÎªµ¥Î»ÏòÎÄ¼þÐ´ÈëÊý¾Ý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
//	mStopIfError( i );
//	
//	strcpy((char *)mCmdParam.ByteWrite.mByteBuffer, "abcde");
//	mCmdParam.ByteWrite.mByteCount = strlen("abcde");                   /* Ö¸¶¨±¾´ÎÐ´ÈëµÄ×Ö½ÚÊý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
//	i = CH375ByteWrite( );                                /* ÒÔ×Ö½ÚÎªµ¥Î»ÏòÎÄ¼þÐ´ÈëÊý¾Ý,µ¥´Î¶ÁÐ´µÄ³¤¶È²»ÄÜ³¬¹ýMAX_BYTE_IO */
//	mStopIfError( i );
//	}
//	
//	mCmdParam.Close.mUpdateLen = 1;                       /* ²»Òª×Ô¶¯¼ÆËãÎÄ¼þ³¤¶È,Èç¹û×Ô¶¯¼ÆËã,ÄÇÃ´¸Ã³¤¶È×ÜÊÇCH375vSectorSizeµÄ±¶Êý */
//	i = CH375FileClose( );
//	mStopIfError( i );		
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

