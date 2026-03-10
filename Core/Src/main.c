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
#include <string.h>
#include <ctype.h>
#include "PS2X_lib.h"
#include "log_helper.h"
#include "diff_drive.h"
#include "string_helper.h"

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
uint32_t ms_tick_count = 0;

#define PID_CMD_BUFFER_LEN 128
static char pid_rx_buffer[PID_CMD_BUFFER_LEN];
static uint16_t pid_rx_index = 0;

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

static void process_pid_command(const char *cmd)
{
    float kp, ki, kd;
    if (parse_pid_command(cmd, &kp, &ki, &kd)) {
        diff_drive_set_pid(kp, ki, kd);
        printf("PID update success!\r\n");
    } else {
        if (cmd != NULL && cmd[0] != '\0') {
            printf("PID parse error: '%s'\r\n", cmd);
        }
    }
}

void usart2_interrupt_handler(uint8_t data)
{
    // Echo received data immediately
    USART2_sendChar(data);

    if (data == '\r') {
        // ignore CR, parse on LF
        return;
    }

    if (data == '\n') {
        // complete command detected
        pid_rx_buffer[pid_rx_index] = '\0';
        if (pid_rx_index > 0) {
            process_pid_command(pid_rx_buffer);
        }
        pid_rx_index = 0;
        return;
    }

    if (pid_rx_index < PID_CMD_BUFFER_LEN - 1) {
        pid_rx_buffer[pid_rx_index++] = (char)data;
    } else {
        // overflow, reset buffer to avoid hang
        pid_rx_index = 0;
        memset(pid_rx_buffer, 0, PID_CMD_BUFFER_LEN);
        printf("PID buffer overflow\r\n");
    }
}

void tim7_interrupt_handler(void)
{
    // Trigger ADC conversion
    ADC1_triggerConvert();

    // Toggle LED LD3
    LL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);

    // // Printf encoder
    // encoder_count = getEncoderCount(MOTOR_L);
    // LOG_DBG("Left Encoder Count: %ld\r\n", encoder_count);
    // encoder_count = getEncoderCount(MOTOR_R);
    // LOG_DBG("Right Encoder Count: %ld\r\n", encoder_count);
}

// Tim10 interrupt for control motor with PID at 100 Hz
void tim10_interrupt_handler(void)
{
    // Update motor control loop
    diff_drive_update(); // 100 Hz update rate
    // Toggle PID_CLK pin for debugging with oscilloscope
    LL_GPIO_TogglePin(PID_CLK_GPIO_Port, PID_CLK_Pin);
}

void sys_tick_handler(void)
{
    ms_tick_count++;
}

uint32_t get_ms_tick_count(void)
{
    return ms_tick_count;
}

uint64_t get_us_tick_count(void)
{
    // Return microsecond tick count based on ms_tick_count and get current SysTick value for finer resolution
     uint32_t systick_val = SysTick->VAL; // Current SysTick counter value
     uint32_t systick_reload = SysTick->LOAD; // SysTick reload value

     // Calculate elapsed microseconds since last millisecond tick
    uint64_t us_tick_count = (systick_reload - systick_val) / (SystemCoreClock / 1000000);
    return (ms_tick_count * 1000) + (us_tick_count % 1000);
}

void delay_us(uint32_t us)
{
    uint64_t start = get_us_tick_count();
    while ((get_us_tick_count() - start) < us);
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
  MX_TIM10_Init();
  /* USER CODE BEGIN 2 */
    // Initialize servos
    // Servo_Init();

    // Initialize system
    diff_drive_init();

    // Enable interrupts after setup
    __enable_irq();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    // ADC1_triggerConvert();

    // Set log level
    set_log_level(LOG_LEVEL_INF);

    // Init PS2X
    for (int i =0; i<10; i++)
    {

        if (ps2x_init() == PS2X_SUCCESS) {
            // ps2x_config_mode(PS2X_MODE_PRESSURES);
            LOG_DBG("PS2X Controller Initialized Successfully\r\n");
            break;
        } else if (i == 9) {
            LOG_ERR("Failed to Initialize PS2X Controller\r\n");
        }
    }

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

    // // convert ADC to angle (0-180)
    // angle = (adc_value * 180) / 4095;
    // Servo_setAngle(SERVO1, angle);
    // LL_mDelay(300);

    float linear_vel = 0.0f;
    float angular_vel = 0.0f;

    // Read PS2 controller state and print button states
    ps2x_read_gamepad();
    // Get data from PS2X and print for debugging
    PS2X_State ps2_state = ps2x_getAllData();

    // Check button l1,r1 or l2, r2 is pressed to allow control robot
    if (ps2_state.btn2.l1 == PS2X_BTN_ACTIVE || ps2_state.btn2.r1 == PS2X_BTN_ACTIVE ||
        ps2_state.btn2.l2 == PS2X_BTN_ACTIVE || ps2_state.btn2.r2 == PS2X_BTN_ACTIVE) {

            // Get joystick Left Y used for linear velocity control
        int16_t ly = 127 - ps2_state.ly; // Range: 0-255, 127 is center
        // Map ly to linear velocity (-0.3 to 0.3 m/s)
        linear_vel = ((float)ly / 128.0f) * MAX_LINEAR_VEL;
        if (linear_vel < 0.05f && linear_vel > -0.05f) {
            linear_vel = 0.0f; // Deadzone for joystick
        }

        // Get joystick Right X used for angular velocity control
        int16_t rx = 128 - ps2_state.rx; // Range: 0-255, 128 is center
        // Map rx to angular velocity (-1.0 to 1.0 rad/s)
        angular_vel = ((float)rx / 128.0f) * MAX_ANGULAR_VEL;
        if (angular_vel < 0.1f && angular_vel > -0.1f) {
            angular_vel = 0.0f; // Deadzone for joystick
        }

        // Printf LY and RX for debugging
        printf("Joystick LY: %d, RX: %d\r\n", ly, rx);


        // Get target RPM for debugging
        float target_rpm = getTargetRPM(MOTOR_L);
        float curr_rpm = getCurrentRPM(MOTOR_L);
        printf("left_rpm: %d -> %d\r\n",  (int)target_rpm, (int)curr_rpm);

        target_rpm = getTargetRPM(MOTOR_R);
        curr_rpm = getCurrentRPM(MOTOR_R);
        printf("right_rpm: %d -> %d\r\n",  (int)target_rpm, (int)curr_rpm);
    }

    // Set robot velocity based on joystick input
    // diff_drive_set_velocity(linear_vel, angular_vel);

    // printf("PS2X Mode: 0x%02X, Type: 0x%02X\r\n", ps2_state.mode, ps2_state.type);
    // printf("Joystick Left: X=%d Y=%d\r\n", ps2_state.lx, ps2_state.ly);
    // printf("Joystick Right: X=%d Y=%d\r\n", ps2_state.rx, ps2_state.ry);
    // if (ps2_state.btn1.up == PS2X_BTN_ACTIVE) {
    //     printf("Up button is pressed\r\n");
    // }

    float duty_cycle = (adc_value * 100.0f) / 4095.0f; // Convert ADC value to percentage
    // int duty_int = (int)duty_cycle;
    // int duty_frac = (int)((duty_cycle - duty_int) * 100);
    // printf("Duty Cycle: %d.%02d%%\n", duty_int, duty_frac);

    // Set RPM for motors left according to ADC valueto test PID control
    setMotorRPM(MOTOR_L, duty_cycle * 2);
    float target_rpm = getTargetRPM(MOTOR_L);
    float curr_rpm = getCurrentRPM(MOTOR_L);
    printf("set:%d, curr:%d\r\n",  (int)target_rpm, (int)curr_rpm);

    // if (duty_cycle < 5.0) {
    //     // Stop robot
    //     diff_drive_stop();
    // } else {
    //     // Set robot velocity based on duty cycle (0.5 m/s max)
    //     float linear_vel = (duty_cycle / 100.0f) * MAX_LINEAR_VEL; // Scale to max speed
    //     diff_drive_set_velocity(linear_vel, 0.0f);                 // Move forward with no rotation
    //     // int _int = (int)linear_vel;
    //     // int _frac = (int)((linear_vel - _int) * 100);
    //     // printf("Linear Velocity: %d.%02d m/s\n", _int, _frac);
    // }

    LL_mDelay(50);
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
