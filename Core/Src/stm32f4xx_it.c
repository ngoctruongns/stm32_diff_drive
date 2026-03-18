/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ws2812.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
    sys_tick_handler();
  /* USER CODE END SysTick_IRQn 0 */

  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 stream1 global interrupt.
  */
void DMA1_Stream1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream1_IRQn 0 */

  if (LL_DMA_IsActiveFlag_HT1(DMA1) && LL_DMA_IsEnabledIT_HT(DMA1, LL_DMA_STREAM_1)) {
    LL_DMA_ClearFlag_HT1(DMA1);
    usart3_dma_rx_interrupt_handler();
  }

  if (LL_DMA_IsActiveFlag_TC1(DMA1) && LL_DMA_IsEnabledIT_TC(DMA1, LL_DMA_STREAM_1)) {
    LL_DMA_ClearFlag_TC1(DMA1);
    usart3_dma_rx_interrupt_handler();
  }

  if (LL_DMA_IsActiveFlag_TE1(DMA1)) {
    LL_DMA_ClearFlag_TE1(DMA1);
  }

  if (LL_DMA_IsActiveFlag_DME1(DMA1)) {
    LL_DMA_ClearFlag_DME1(DMA1);
  }

  if (LL_DMA_IsActiveFlag_FE1(DMA1)) {
    LL_DMA_ClearFlag_FE1(DMA1);
  }

  /* USER CODE END DMA1_Stream1_IRQn 0 */
  /* USER CODE BEGIN DMA1_Stream1_IRQn 1 */

  /* USER CODE END DMA1_Stream1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream3 global interrupt.
  */
void DMA1_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream3_IRQn 0 */

  if (LL_DMA_IsActiveFlag_TC3(DMA1) && LL_DMA_IsEnabledIT_TC(DMA1, LL_DMA_STREAM_3)) {
    LL_DMA_ClearFlag_TC3(DMA1);
    usart3_dma_tx_interrupt_handler();
  }

  if (LL_DMA_IsActiveFlag_TE3(DMA1)) {
    LL_DMA_ClearFlag_TE3(DMA1);
    usart3_dma_tx_interrupt_handler();
  }

  if (LL_DMA_IsActiveFlag_DME3(DMA1)) {
    LL_DMA_ClearFlag_DME3(DMA1);
    usart3_dma_tx_interrupt_handler();
  }

  if (LL_DMA_IsActiveFlag_FE3(DMA1)) {
    LL_DMA_ClearFlag_FE3(DMA1);
    usart3_dma_tx_interrupt_handler();
  }

  /* USER CODE END DMA1_Stream3_IRQn 0 */
  /* USER CODE BEGIN DMA1_Stream3_IRQn 1 */

  /* USER CODE END DMA1_Stream3_IRQn 1 */
}

/**
  * @brief This function handles ADC1, ADC2 and ADC3 global interrupts.
  */
void ADC_IRQHandler(void)
{
  /* USER CODE BEGIN ADC_IRQn 0 */
    // Check for end of conversion flag
    if (LL_ADC_IsActiveFlag_EOCS(ADC1)) {
        // Clear the end of conversion flag
        LL_ADC_ClearFlag_EOCS(ADC1);

        // Read the ADC conversion result
        uint16_t adc_value = LL_ADC_REG_ReadConversionData12(ADC1);
        adc1_interrupt_handler(adc_value);
    }
  /* USER CODE END ADC_IRQn 0 */
  /* USER CODE BEGIN ADC_IRQn 1 */

  /* USER CODE END ADC_IRQn 1 */
}

/**
  * @brief This function handles TIM1 update interrupt and TIM10 global interrupt.
  */
void TIM1_UP_TIM10_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 0 */
    if (LL_TIM_IsActiveFlag_UPDATE(TIM10)) {
        // Clear update interrupt flag
        LL_TIM_ClearFlag_UPDATE(TIM10);
        tim10_interrupt_handler();
    }

  /* USER CODE END TIM1_UP_TIM10_IRQn 0 */
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
  if (LL_USART_IsActiveFlag_RXNE(USART2)) {
    uint8_t data = LL_USART_ReceiveData8(USART2);
    usart2_interrupt_handler(data);
    // RXNE flag is cleared by reading USART_RDR
  }

  if (LL_USART_IsActiveFlag_ORE(USART2)) {
    // Overrun occurred, clear
    LL_USART_ClearFlag_ORE(USART2);
  }
  /* USER CODE END USART2_IRQn 0 */
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */

  if (LL_USART_IsActiveFlag_IDLE(USART3) && LL_USART_IsEnabledIT_IDLE(USART3)) {
    volatile uint32_t dummy;
    dummy = USART3->SR;
    dummy = USART3->DR;
    (void)dummy;
    usart3_idle_interrupt_handler();
  }

  if (LL_USART_IsActiveFlag_ORE(USART3)) {
    LL_USART_ClearFlag_ORE(USART3);
  }

  /* USER CODE END USART3_IRQn 0 */
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles TIM5 global interrupt.
  */
void TIM5_IRQHandler(void)
{
  /* USER CODE BEGIN TIM5_IRQn 0 */

  /* USER CODE END TIM5_IRQn 0 */
  /* USER CODE BEGIN TIM5_IRQn 1 */

  /* USER CODE END TIM5_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */
    if (LL_TIM_IsActiveFlag_UPDATE(TIM7)) {
        // Clear update interrupt flag
        LL_TIM_ClearFlag_UPDATE(TIM7);
        tim7_interrupt_handler();
    }
  /* USER CODE END TIM7_IRQn 0 */
  /* USER CODE BEGIN TIM7_IRQn 1 */

  /* USER CODE END TIM7_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream1 global interrupt.
  */
void DMA2_Stream1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream1_IRQn 0 */
    WS2812_DMA_IRQHandler();
  /* USER CODE END DMA2_Stream1_IRQn 0 */
  /* USER CODE BEGIN DMA2_Stream1_IRQn 1 */

  /* USER CODE END DMA2_Stream1_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
