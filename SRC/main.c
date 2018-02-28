/*********************************************************************
*                Copyright (C), 2015-2016, Supersonics. Co., Ltd.
*                        �迹��������������
*
*                          Ӳ��ƽ̨: xxx
*                          �� о Ƭ: STM32F103
*                          �� Ŀ ��: xxx
**********************************************************************
*�ļ���: main.c
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

#include "main.h"
#include "control.h"

#include "delay.h"
#include "sys.h"
#include "ch375.h"



ErrorStatus HSEStartUpStatus;

u16 count=0;//   DGUT
u16 count0=0; 

__IO uint16_t ADCConvertedValue[2]={0};       /*��DMA�����Ӧ��ADC1_CH8,��ADC1_CH9ͨ��ֵ*/

/*********************************************************************
* �������ã���ʱ
* ����������kCount
* ��������ֵ����
**********************************************************************/
void Delayus(__IO uint32_t kCount)
{
  for (; kCount != 0; kCount--)
	{}
}

/*********************************************************************
* �������ã�ʱ�����ã�����ϵͳĬ������
* ������������
* ��������ֵ����
**********************************************************************/
void RCC_Configuration(void)
{
  RCC_DeInit();                                  /*��λRCC�ⲿ�豸�Ĵ�����Ĭ��ֵ*/

  RCC_HSEConfig(RCC_HSE_ON);                     /*���ⲿ���پ���*/

  HSEStartUpStatus = RCC_WaitForHSEStartUp();    /*�ȴ��ⲿ����ʱ��׼����*/

  if(HSEStartUpStatus == SUCCESS)                /*�ⲿ����ʱ���Ѿ�׼���*/
  {    
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);   
    FLASH_SetLatency(FLASH_Latency_2); 
    RCC_HCLKConfig(RCC_SYSCLK_Div1);             /*����AHB(HCLK)ʱ��=SYSCLK*/ 
    RCC_PCLK2Config(RCC_HCLK_Div1);              /*����APB2(PCLK2)��=AHBʱ��*/
    RCC_PCLK1Config(RCC_HCLK_Div1);              /*����APB1(PCLK1)��=AHB ʱ��*/

    RCC_ADCCLKConfig(RCC_PCLK2_Div4);            /*����ADCʱ��=PCLK2 1/4*/ 
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);  /*����PLLʱ�� == �ⲿ���پ���ʱ��*9*/   
    RCC_ADCCLKConfig(RCC_PCLK2_Div4);            /*����ADCʱ��= PCLK2/4*/

    RCC_PLLCmd(ENABLE);                          /*ʹ��PLLʱ��*/

    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)  /*�ȴ�PLLʱ�Ӿ���*/
    {
    }

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);   /*����ϵͳʱ�� = PLLʱ��*/

    while(RCC_GetSYSCLKSource() != 0x08)         /*���PLLʱ���Ƿ���Ϊϵͳʱ��*/
    {
    }
  }

}

/*******************************************************************
* �������ã�CPU �ܽ�����
* ������������
* ��������ֵ����
********************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  	   /*GPIO�ṹ�嶨��*/

	/*ʹ��ʹ�õ���GPIOʱ��*/
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
	
 	/*��������ʼ������  --  PA11*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11;	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure); 

 	/*��崮��ָʾ������  --  PA7*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	/*************PB6 ��ѹ��λ·*********************/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*************PB7 ������λ·*********************/
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
	
	/**************PB8--PB15 �迹�л�*********************/
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
* �������ã���������
* ������������
* ��������ֵ����
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
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); /*�����ж�ʹ��*/

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
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); /*�����ж�ʹ��*/

	/* Enable USART */
	USART_Cmd(USART2, ENABLE);
}

/*******************************************************************
* �������ã�DMA�������ã�����ADC���ݵĴ���
* ������������
* ��������ֵ����
********************************************************************/
void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure;                                //DMA��ʼ���ṹ������
	/* Enable DMA1 clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		            //ʹ��DMAʱ��

	/* DMA1 channel1 configuration ----------------------------------------------*/
	DMA_DeInit(DMA1_Channel1);		                                    //����DMA1�ĵ�һͨ��
	DMA_InitStructure.DMA_PeripheralBaseAddr = DR_ADDRESS;		        //DMA��Ӧ���������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCConvertedValue;   //�ڴ�洢����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	              //DMA��ת��ģʽΪSRCģʽ����������Ƶ��ڴ�
	DMA_InitStructure.DMA_BufferSize = 2;		                          //DMA�����С��2��
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//����һ�����ݺ��豸��ַ��ֹ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;          	//�رս���һ�����ݺ�Ŀ���ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;     //�����������ݿ��Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  //DMA�������ݳߴ磬HalfWord����Ϊ16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                      //ת��ģʽ��ѭ������ģʽ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	                 //DMA���ȼ���
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		                     //M2Mģʽ����
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);          
	/* Enable DMA1 channel1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

/**********************************************************************
* �������ã�ADC����
* ������������
* ��������ֵ����
***********************************************************************/
void ADC_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure;        /*ADC��ʼ���ṹ����*/

  /* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	               /*ʹ��ADC��GPIOCʱ��*/
  /* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;		               /*������ת��ģʽ*/
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;		                     /*����ɨ��ģʽ*/
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                   /*��������ת��ģʽ*/
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	 /*ADC�ⲿ���أ��ر�״̬*/
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;               /*���뷽ʽ,ADCΪ12λ�У��Ҷ��뷽ʽ*/
	ADC_InitStructure.ADC_NbrOfChannel = 2;	                             /*����ͨ������1��*/
	ADC_Init(ADC1, &ADC_InitStructure);
	/* ADC1 regular channel15 configuration ����IIʹ��PC5��ΪAD PINͨ��Ϊ15*/ 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5);	 /*ADCͨ���飬 ��8��ͨ�� ����˳��1��ת��ʱ��71.5ʱ�� 71.5+12.5=84��ת������  */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_239Cycles5);	 /*ADCͨ���飬 ��9��ͨ�� ����˳��2��ת��ʱ��71.5ʱ�� 6us����һ��ֵ*/
												
	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);	  
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);  

	/* Enable ADC1 reset calibaration register */   
	ADC_ResetCalibration(ADC1);	                                   /*����У׼*/
 /* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));                    /*�ȴ�����У׼���*/
	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);		                                 /*��ʼУ׼*/
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));	                       /*�ȴ�У׼���*/
	/* Start ADC1 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	                       /*����ת����ʼ��ADCͨ��DMA��ʽ���ϵĸ���RAM����*/
}

/*******************************************************************
* �������ã��ж�����������
* ������������
* ��������ֵ����
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
  
  // С������
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
* �������ã���ʱ����ʼ��
* ������������
* ��������ֵ����
********************************************************************/

void TIM_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_BaseInitStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	
	TIM_DeInit(TIM2);
	/***72M�¶�ʱֵ�ļ��㣨��1+Ԥ��ƵTIM_Prescaler��/72*(1+��ʱ����TIM_Period)��*/ 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);            /*����RCC��ʹ��TIMx */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);            //RCC->APB1ENR|=1<<1;  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  /* Time Base configuration */
  TIM_BaseInitStructure.TIM_Prescaler =35;                        /*ʱ��Ԥ��Ƶ�� ����:ʱ��Ƶ��=36/(ʱ��Ԥ��Ƶ+1)*/  
  TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Down;   /*��ʱ��ģʽ ���¼���*/
  TIM_BaseInitStructure.TIM_Period = 2000;                        //��ʱ1MS//�Զ���װ�ؼĴ������ڵ�ֵ(��ʱʱ��)�ۼ� 0xFFFF��Ƶ�ʺ���������»����ж�(Ҳ��˵��ʱʱ�䵽)
  TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;         /*ʱ��ָ�ֵ*/
	TIM_BaseInitStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &TIM_BaseInitStructure);                 /*��ʼ����ʱ��2*/

	TIM_Cmd(TIM2, DISABLE);  
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);                      /*ʹ��CC2�ж�����*/
	
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
  TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);	                          /*Channel 1�����ж�ʹ��*/
  TIM_ITConfig(TIM4, TIM_IT_CC2, ENABLE);	                          /*Channel 2�����ж�ʹ��*/
	TIM4->CCER &= (uint16_t)~((uint16_t)TIM_CCER_CC1E);		            /*��ֹһͨ������*/
	TIM4->CCER &= (uint16_t)~((uint16_t)TIM_CCER_CC2E);		            /*��ֹ��ͨ������*/
	
	
	// С������
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
	TIM_BaseInitStructure.TIM_Period = 2000; 	//�Զ���װ��ֵ
	TIM_BaseInitStructure.TIM_Prescaler=35;  //��ʱ����Ƶ
	TIM_BaseInitStructure.TIM_CounterMode=TIM_CounterMode_Down; //����ģʽ
	TIM_BaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_BaseInitStructure);//��ʼ��TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��2�����ж�
	TIM_Cmd(TIM3,DISABLE); //ʹ�ܶ�ʱ��3
	
}


/*******************************************************************
* �������ã�CPU�������ʼ��
* ������������
* ��������ֵ����
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
* �������ã����������
* ������������
* ��������ֵ����
********************************************************************/
int main ()
{
	qsize  SIZE = 0;
	
	CPU_Init();
	AD9833_Init();
  queue_reset();					              /*��մ��ڽ��ջ�����*/
	SetDeviceReset();				              /*��λ������*/
	Delayus(3000000);		                  /*��ʱ�ȴ���������ʼ�����,����ȴ�300ms*/
  iniuserctr();					                /*��ʼ���û��ؼ�*/
	Delayus(3000000);
	
	/* С������ */
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

		SIZE = queue_find_cmd(cmd_buffer,CMD_MAX_BUFFER);         /*�ӻ������л�ȡһ��ָ��*/
    if(SIZE<=0)                                               /*û�н��յ�ָ��*/
			continue;

		Message_Deal(SIZE);					                              /*�����źŴ�����*/
		
		OK_Button();                                              /*�ж�������ť�Ƿ���*/	
		Stop_Button();                                            /*�ж�ֹͣ��ť�Ƿ���*/
		ClearScreen_Button();                                     /*�ж�������ť�Ƿ���*/
		SaveData_Button();                                        /*�жϱ������ݰ�ť�Ƿ���*/
		SavePic_Button();                                         /*�жϱ���ͼƬ��ť�Ƿ���*/
  }
}
