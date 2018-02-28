/*********************************************************************
*                Copyright (C), 2015-2016, Supersonics. Co., Ltd.
*                        阻抗分析仪驱动程序
*
*                          硬件平台: xxx
*                          主 芯 片: STM32F103
*                          项 目 组: xxx
**********************************************************************
*文件名: main.c
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

#include "main.h"
#include "control.h"

#include "delay.h"
#include "sys.h"
#include "ch375.h"



ErrorStatus HSEStartUpStatus;

u16 count=0;//   DGUT
u16 count0=0; 

__IO uint16_t ADCConvertedValue[2]={0};       /*是DMA传输对应的ADC1_CH8,和ADC1_CH9通道值*/

/*********************************************************************
* 函数作用：延时
* 函数参数：kCount
* 函数返回值：无
**********************************************************************/
void Delayus(__IO uint32_t kCount)
{
  for (; kCount != 0; kCount--)
	{}
}

/*********************************************************************
* 函数作用：时钟配置，采用系统默认配置
* 函数参数：无
* 函数返回值：无
**********************************************************************/
void RCC_Configuration(void)
{
  RCC_DeInit();                                  /*复位RCC外部设备寄存器到默认值*/

  RCC_HSEConfig(RCC_HSE_ON);                     /*打开外部高速晶振*/

  HSEStartUpStatus = RCC_WaitForHSEStartUp();    /*等待外部高速时钟准备好*/

  if(HSEStartUpStatus == SUCCESS)                /*外部高速时钟已经准别好*/
  {    
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);   
    FLASH_SetLatency(FLASH_Latency_2); 
    RCC_HCLKConfig(RCC_SYSCLK_Div1);             /*配置AHB(HCLK)时钟=SYSCLK*/ 
    RCC_PCLK2Config(RCC_HCLK_Div1);              /*配置APB2(PCLK2)钟=AHB时钟*/
    RCC_PCLK1Config(RCC_HCLK_Div1);              /*配置APB1(PCLK1)钟=AHB 时钟*/

    RCC_ADCCLKConfig(RCC_PCLK2_Div4);            /*配置ADC时钟=PCLK2 1/4*/ 
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);  /*配置PLL时钟 == 外部高速晶体时钟*9*/   
    RCC_ADCCLKConfig(RCC_PCLK2_Div4);            /*配置ADC时钟= PCLK2/4*/

    RCC_PLLCmd(ENABLE);                          /*使能PLL时钟*/

    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)  /*等待PLL时钟就绪*/
    {
    }

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);   /*配置系统时钟 = PLL时钟*/

    while(RCC_GetSYSCLKSource() != 0x08)         /*检查PLL时钟是否作为系统时钟*/
    {
    }
  }

}

/*******************************************************************
* 函数作用：CPU 管脚配置
* 函数参数：无
* 函数返回值：无
********************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  	   /*GPIO结构体定义*/

	/*使能使用到的GPIO时钟*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); 
	
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
 	/*蜂鸣器初始化配置  --  PA11*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure); 

 	/*面板串口指示工作灯  --  PA7*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	/*************PB6 电压相位路*********************/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*************PB7 电流相位路*********************/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	/*************PB11 FSYNC*********************/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*************PB13 SCLK*********************/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	/*************PB14 SDATA*********************/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	/**************PB8--PB15 阻抗切换*********************/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12 | GPIO_Pin_11 | GPIO_Pin_10| GPIO_Pin_9 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);	
	
	GPIO_SetBits(GPIOA, GPIO_Pin_7);
	
	
	
	//USART2
	/* Configure USART Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Configure USART Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);
}

/*******************************************************************
* 函数作用：串口配置
* 函数参数：无
* 函数返回值：无
********************************************************************/
void USART_Configuration(void)
{
 	/* USARTx configured as follow:
     - BaudRate = 115200 baud  
     - Word Length = 8 Bits
     - One Stop Bit
     - No parity
     - Hardware flow control disabled (RTS and CTS signals)
     - Receive and transmit enabled
  */
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	USART_DeInit(USART1);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode =   USART_Mode_Tx|USART_Mode_Rx;

	/* USART configuration */
	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); /*接收中断使能*/

	/* Enable USART */
	USART_Cmd(USART1, ENABLE);
	
	//USART2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	USART_DeInit(USART2);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode =   USART_Mode_Tx|USART_Mode_Rx;

	/* USART configuration */
	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); /*接收中断使能*/

	/* Enable USART */
	USART_Cmd(USART2, ENABLE);
}

/*******************************************************************
* 函数作用：DMA总线配置，用于ADC数据的传输
* 函数参数：无
* 函数返回值：无
********************************************************************/
void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;                                //DMA初始化结构体声明
	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		            //使能DMA时钟

	/* DMA1 channel1 configuration ----------------------------------------------*/
	DMA_DeInit(DMA1_Channel1);		                                    //开启DMA1的第一通道
	DMA_InitStructure.DMA_PeripheralBaseAddr = DR_ADDRESS;		        //DMA对应的外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCConvertedValue;   //内存存储基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	              //DMA的转换模式为SRC模式，由外设搬移到内存
	DMA_InitStructure.DMA_BufferSize = 2;		                          //DMA缓存大小，2个
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//接收一次数据后，设备地址禁止后移
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;          	//关闭接收一次数据后，目标内存地址后移
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;     //定义外设数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  //DMA搬移数据尺寸，HalfWord就是为16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                      //转换模式，循环缓存模式。
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	                 //DMA优先级高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		                     //M2M模式禁用
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);          
	/* Enable DMA1 channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

/**********************************************************************
* 函数作用：ADC配置
* 函数参数：无
* 函数返回值：无
***********************************************************************/
void ADC_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure;        /*ADC初始化结构体声*/

  /* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	               /*使能ADC和GPIOC时钟*/
  /* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		               /*独立的转换模式*/
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;		                     /*开启扫描模式*/
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                   /*开启连续转换模式*/
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	 /*ADC外部开关，关闭状态*/
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;               /*对齐方式,ADC为12位中，右对齐方式*/
	ADC_InitStructure.ADC_NbrOfChannel = 2;	                             /*开启通道数，1个*/
	ADC_Init(ADC1, &ADC_InitStructure);
	/* ADC1 regular channel15 configuration 神舟II使用PC5作为AD PIN通道为15*/ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5);	 /*ADC通道组， 第8个通道 采样顺序1，转换时间71.5时钟 71.5+12.5=84个转化周期  */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_239Cycles5);	 /*ADC通道组， 第9个通道 采样顺序2，转换时间71.5时钟 6us采样一个值*/
												
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);	  
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);  

	/* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);	                                   /*重新校准*/
 /* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));                    /*等待重新校准完成*/
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);		                                 /*开始校准*/
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));	                       /*等待校准完成*/
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	                       /*连续转换开始，ADC通过DMA方式不断的更新RAM区。*/
}

/*******************************************************************
* 函数作用：中断向量的配置
* 函数参数：无
* 函数返回值：无
********************************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
	#ifdef  VECT_TAB_RAM
  /* Set the Vector Table base location at 0x20000000 */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
	#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	#endif
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  /* Enable the TIM2 Interrupt */

  /* Enable the USART1 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  /* Enable the USART2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	/* Enable the TIM2 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  // 小板新增
  /* Enable the TIM3 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	/* Enable the TIM4 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************
* 函数作用：定时器初始化
* 函数参数：无
* 函数返回值：无
********************************************************************/

void TIM_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	
	TIM_DeInit(TIM2);
	/***72M下定时值的计算（（1+预分频TIM_Prescaler）/72*(1+定时周期TIM_Period)）*/ 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);            /*配置RCC，使能TIMx */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);            //RCC->APB1ENR|=1<<1;  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  /* Time Base configuration */
  TIM_BaseInitStructure.TIM_Prescaler =35;                        /*时钟预分频数 例如:时钟频率=36/(时钟预分频+1)*/  
  TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Down;   /*定时器模式 向下计数*/
  TIM_BaseInitStructure.TIM_Period = 2000;                        //定时1MS//自动重装载寄存器周期的值(定时时间)累计 0xFFFF个频率后产生个更新或者中断(也是说定时时间到)
  TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;         /*时间分割值*/
	TIM_BaseInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &TIM_BaseInitStructure);                 /*初始化定时器2*/

	TIM_Cmd(TIM2, DISABLE);  
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);                      /*使能CC2中断请求*/
	
	/**************TIM4 Time base configuration********************/
	TIM_BaseInitStructure.TIM_Period = (65535-1);	
	TIM_BaseInitStructure.TIM_Prescaler = 1-1;	                    /*10KHz -- 100us*/
	TIM_BaseInitStructure.TIM_ClockDivision = 0;
	TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_BaseInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1 ;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x5;
	TIM_ICInit(TIM4, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0x5;
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	
	/* Disable the Master/Slave Mode */
	TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Disable);

	TIM_Cmd(TIM4, ENABLE);
  TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);	                          /*Channel 1捕获中断使能*/
  TIM_ITConfig(TIM4, TIM_IT_CC2, ENABLE);	                          /*Channel 2捕获中断使能*/
	TIM4->CCER &= (uint16_t)~((uint16_t)TIM_CCER_CC1E);		            /*禁止一通道捕获*/
	TIM4->CCER &= (uint16_t)~((uint16_t)TIM_CCER_CC2E);		            /*禁止二通道捕获*/
	
	
	// 小板新增
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
	TIM_BaseInitStructure.TIM_Period = 2000; 	//自动重装载值
	TIM_BaseInitStructure.TIM_Prescaler=35;  //定时器分频
	TIM_BaseInitStructure.TIM_CounterMode=TIM_CounterMode_Down; //向下模式
	TIM_BaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_BaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器2更新中断
	TIM_Cmd(TIM3,DISABLE); //使能定时器3
	
}


/*******************************************************************
* 函数作用：CPU配置与初始化
* 函数参数：无
* 函数返回值：无
********************************************************************/
void CPU_Init(void)
{
	RCC_Configuration();
	GPIO_Configuration();
	USART_Configuration();
	DMA_Configuration();
	ADC_Configuration();
	NVIC_Configuration();
	TIM_Configuration();
	
	GPIO_ResetBits(GPIOE, GPIO_Pin_15);
	GPIO_SetBits(GPIOE, GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
}



/*******************************************************************
* 函数作用：主函数入口
* 函数参数：无
* 函数返回值：无
********************************************************************/
int main ()
{
	qsize  SIZE = 0;
	
	CPU_Init();
	AD9833_Init();
  queue_reset();					              /*清空串口接收缓冲区*/
	SetDeviceReset();				              /*复位串口屏*/
	Delayus(3000000);		                  /*延时等待串口屏初始化完毕,必须等待300ms*/
  iniuserctr();					                /*初始化用户控件*/
	Delayus(3000000);
	
	/* 小板新增 */
	delay_init();
	CH375_Configuration();
	CH375_Init();
	
  while (1)
  {
		if(Ok_Control_Flag == 0)
		{
			Ok_Control_Flag = 1;
			ShowControl(0,3,1);
		}

		SIZE = queue_find_cmd(cmd_buffer,CMD_MAX_BUFFER);         /*从缓冲区中获取一条指令*/
    if(SIZE<=0)                                               /*没有接收到指令*/
			continue;

		Message_Deal(SIZE);					                              /*接收信号处理函数*/
		
		OK_Button();                                              /*判断启动按钮是否按下*/	
		Stop_Button();                                            /*判断停止按钮是否按下*/
		ClearScreen_Button();                                     /*判断清屏按钮是否按下*/
		SaveData_Button();                                        /*判断保存数据按钮是否按下*/
		SavePic_Button();                                         /*判断保存图片按钮是否按下*/
  }
}
