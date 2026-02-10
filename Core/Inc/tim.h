/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define MOTOR_PWM1 LL_TIM_CHANNEL_CH1
#define MOTOR_PWM2 LL_TIM_CHANNEL_CH2

// Servo Identifiers
typedef enum {
    SERVO1 = 0,
    SERVO2 = 1,
    SERVO_MAX_ID
} Servo_ID;

// Struct for servo configuration
typedef struct {
    Servo_ID id;
    uint32_t timer_channel; // Timer channel used for PWM
    int16_t min_angle;      // Minimum angle (usually 0 degrees)
    int16_t max_angle;      // Maximum angle (usually 180 degrees)
    uint32_t min_pulse;     // Minimum pulse width in microseconds (e.g., 500us)
    uint32_t max_pulse;     // Maximum pulse width in microseconds (e.g., 2500us)
} Servo_Config;

/* USER CODE END Private defines */

void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
void MX_TIM5_Init(void);
void MX_TIM7_Init(void);

/* USER CODE BEGIN Prototypes */

/* TIM4 PWM Control Functions  */
void TIM4_enable_PWM(uint32_t Channel);
void TIM4_disable_PWM(uint32_t Channel);
void TIM4_Set_PWM_DutyCycle(uint32_t Channel, uint32_t DutyCycle);

/* Servo Control Functions */
// Control servo motor (0-180 degrees)
// PWM frequency: 50Hz
// Pulse width: 0.5ms (0 deg) to 2.5 ms (180 deg)
void Servo_Init(void);
void Servo_setAngle(Servo_ID servo_id, int16_t angle);

int32_t Encoder_GetCount(void);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

