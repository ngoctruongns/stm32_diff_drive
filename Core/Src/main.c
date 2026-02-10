/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "adc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t is_first_capture = 1;
uint32_t last_capture = 0;
uint32_t duty_cycle = 0;
uint32_t pulse_width = 0;
uint16_t adc_value;
int16_t angle;
float temperature;
char usart_buffer[50];
int32_t encoder_count = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// int _write(int file, char *ptr, int len)
//{
//     for (int i = 0; i < len; i++) {
//         ITM_SendChar((*ptr++));
//     }
//     return len;
// }

int __io_putchar(int ch)
{
    USART2_sendChar(ch);
    return ch;
}

// Handler for input capture, calculate duty cycle
void input_capture_tim3_cc1_handler(uint32_t captured_value) {
    if (is_first_capture) {
        last_capture = captured_value;
        is_first_capture = 0;
        return;
    }

    if (captured_value >= last_capture) {
        duty_cycle = captured_value - last_capture;
    } else {
        // Handle timer overflow
        duty_cycle = (0xFFFF - last_capture) + captured_value + 1;
    }

    last_capture = captured_value;

    // Print duty cycle for debugging
    // printf("Captured Duty Cycle: %lu\r\n", duty_cycle);

}

// Measure pulse width on falling edge, used for echo signal from ultrasonic sensor
void input_capture_tim3_cc2_handler(uint32_t falling_value) {
    // Calculate pulse width, when falling edge is captured - rising edge was stored
    if (falling_value >= last_capture) {
        pulse_width = falling_value - last_capture;
    } else {
        // Handle timer overflow
        pulse_width = (0xFFFF - last_capture) + falling_value + 1;
    }

    // float distance = ((float) pulse_width * 0.0343f) / 2.0f; // in cm
    // printf("Distance (cm): %d\r\n", (int) distance);
}

// ADC conversion complete interrupt handler
void adc1_interrupt_handler(uint16_t val)
{
    adc_value = val;
    // printf("ADC Value: %d\r\n", adc_value);
}

float ADC_readTemperature(uint16_t adc_raw)
{
    float vsense;
    float temperature;

    // Convert to Vsense
    vsense = (float)adc_raw * 3300.0f / 4095.0f;

    // Calculate temperature in Celsius
    temperature = ((vsense - 760.0f) / 2.5f) + 25.0f;

    return temperature;
}

void usart2_interrupt_handler(uint8_t data)
{
    // Echo received data back
    USART2_sendChar(data);
}

void tim7_interrupt_handler(void)
{
    // Trigger ADC conversion
    ADC1_triggerConvert();

    // Toggle LED LD3
    LL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);

    // Get encoder count from TIM2
    encoder_count = Encoder_GetCount();
    printf("Encoder Count: %ld, ADC Value: %d\r\n", encoder_count, adc_value);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);

  /* USER CODE BEGIN Init */
  // Disable all interrupts during setup
  __disable_irq();

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
    LL_Init1msTick(168000000);
    LL_SYSTICK_EnableIT();

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_TIM7_Init();
  MX_USART2_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
    // Initialize servos
    Servo_Init();

    // Enable interrupts after setup
    __enable_irq();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    // ADC1_triggerConvert();



  while (1)
  {
    // // Rotate servo1 from 0 to 180 degrees
    // for (int16_t angle = 0; angle <= 180; angle += 1) {
    //     Servo_setAngle(SERVO1, angle);
    //     LL_mDelay(5);
    // }

    // LL_mDelay(500);

    // // Rotate servo1 from 180 to 0 degrees
    // for (int16_t angle = 180; angle >= 0; angle -= 1) {
    //     Servo_setAngle(SERVO1, angle);
    //     LL_mDelay(5);
    // }
    // LL_mDelay(500);

    /* Read ADC value */

    // // Mode1: Independent mode
    // ADC1_triggerConvert();
    // adc_value = ADC1_readValue();

    // // Convert ADC value to temperature in Celsius when using internal temperature sensor
    // temperature = ADC_readTemperature(adc_value);
    // printf("ADC Value: %d, Temperature: %d C\r\n", adc_value, (int)temperature);

    // Send adc value to USART2
    // sprintf(usart_buffer, "ADC Value: %d\r\n", adc_value);
    // USART2_sendString(usart_buffer);
    // USART2_Transmit(usart_buffer, strlen(usart_buffer));

    // convert ADC to angle (0-180)
    angle = (adc_value * 180) / 4095;
    Servo_setAngle(SERVO1, angle);
    LL_mDelay(300);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_5)
  {
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 336, LL_RCC_PLLP_DIV_2);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_Init1msTick(168000000);
  LL_SetSystemCoreClock(168000000);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {}
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
