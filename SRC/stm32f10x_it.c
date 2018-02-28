/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "cmd_queue.h"

/* Global variable------------------------------------------------------------*/
volatile u16 CaptureNumber1 = 0;
s32 angle = 0;
s32 Capture_space = 0;

u32 Time_100Ms = 0;
u32 Time_100Ms_2 = 0;
u16 TimeBase=0;
u16 TimeBase2=0;
__IO uint16_t IC4ReadValue1 = 0, IC4ReadValue2 = 0, IC4ReadValue3 = 0, IC4ReadValue4 = 0;


/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_IOToggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{
 	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)            /*检测制定的中断是否发生   */
	{
//		Time_2Ms ++;
		TimeBase++;
		if(TimeBase>=100)//100ms
		{
			TimeBase = 0;
			Time_100Ms ++;

// 			if(Time_200Ms++ >=5)  //500ms                         /*500ms 灯进行一次闪烁   */
//			{
// 			  	Time_200Ms=0;
//				Led_Proganm_Sun_flag_D12();
//			}
		}
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);                 /*中断函数处理完成，清除中断处理位。   */
}

void TIM3_IRQHandler(void)
{
 	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)            /*检测制定的中断是否发生   */
	{
//		Time_2Ms ++;
		TimeBase2++;
		if(TimeBase2>=100)//100ms
		{
			TimeBase2 = 0;
			Time_100Ms_2 ++;

// 			if(Time_200Ms++ >=5)  //500ms                         /*500ms 灯进行一次闪烁   */
//			{
// 			  	Time_200Ms=0;
//				Led_Proganm_Sun_flag_D12();
//			}
		}
	}
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);                 /*中断函数处理完成，清除中断处理位。   */
}

/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void)                                              /*中断进行定时器捕获   */
{		
	if(CaptureNumber1 == 0)
	{
		if(TIM_GetITStatus(TIM4, TIM_IT_CC1) == SET)
		{
			TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
			IC4ReadValue2 = TIM_GetCapture1(TIM4);
			CaptureNumber1 = 2;
		  TIM4->CCER &= (uint16_t)~((uint16_t)TIM_CCER_CC1E);		            /*禁止一通道捕获   */
			TIM4->CCER |= (uint16_t)TIM_CCER_CC2E;					                  /*同时允许二通道捕获   */
		}	
		TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);		
	}
	
	else if(CaptureNumber1 == 2)
	{
		TIM_ITConfig(TIM4, TIM_IT_CC2, ENABLE); 
		if(TIM_GetITStatus(TIM4, TIM_IT_CC2) == SET)
		{
		 	TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);
			IC4ReadValue3 = TIM_GetCapture2(TIM4);
			CaptureNumber1 = 3;
			TIM4->CCER &= (uint16_t)~((uint16_t)TIM_CCER_CC2E);			          /*禁止二通道捕获   */
		} 	  	
	}

	TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
	TIM_ClearITPendingBit(TIM4, TIM_IT_CC2);
	if(CaptureNumber1 == 3)
	{
		if(IC4ReadValue3 > IC4ReadValue2)
		{
			Capture_space = IC4ReadValue3 - IC4ReadValue2;
		}
		else
		{
			Capture_space = ((0xFFFF - IC4ReadValue2) + IC4ReadValue3);	
		}

		angle = (s32)((double)Capture_space*((double)Current_Fre/1000)*3600/72000000);
		while(angle > 3600)
			angle = angle - 3600;
		if(angle > 1800)
			angle = angle - 1800;
		else
			angle = angle + 1800;
	}
}

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
			uint8_t data = USART_ReceiveData(USART1);              /*中断发生接收来自液晶屏的数据   */
			queue_push(data);
	}
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
