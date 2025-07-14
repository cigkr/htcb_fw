/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define FND3_CLK_Pin GPIO_PIN_2
#define FND3_CLK_GPIO_Port GPIOE
#define LED_CLK_Pin GPIO_PIN_3
#define LED_CLK_GPIO_Port GPIOE
#define FND_nOE_Pin GPIO_PIN_4
#define FND_nOE_GPIO_Port GPIOE
#define ENC_A_Pin GPIO_PIN_0
#define ENC_A_GPIO_Port GPIOC
#define ENC_A_EXTI_IRQn EXTI0_IRQn
#define ENC_B_Pin GPIO_PIN_1
#define ENC_B_GPIO_Port GPIOC
#define ENC_B_EXTI_IRQn EXTI1_IRQn
#define ENC_PB_Pin GPIO_PIN_2
#define ENC_PB_GPIO_Port GPIOC
#define PB_MODE_Pin GPIO_PIN_3
#define PB_MODE_GPIO_Port GPIOC
#define PB_SET_Pin GPIO_PIN_4
#define PB_SET_GPIO_Port GPIOC
#define OUT0_Pin GPIO_PIN_8
#define OUT0_GPIO_Port GPIOE
#define OUT1_Pin GPIO_PIN_9
#define OUT1_GPIO_Port GPIOE
#define OUT2_Pin GPIO_PIN_10
#define OUT2_GPIO_Port GPIOE
#define IN2_Pin GPIO_PIN_10
#define IN2_GPIO_Port GPIOB
#define D0_Pin GPIO_PIN_0
#define D0_GPIO_Port GPIOD
#define D1_Pin GPIO_PIN_1
#define D1_GPIO_Port GPIOD
#define D2_Pin GPIO_PIN_2
#define D2_GPIO_Port GPIOD
#define D3_Pin GPIO_PIN_3
#define D3_GPIO_Port GPIOD
#define D4_Pin GPIO_PIN_4
#define D4_GPIO_Port GPIOD
#define D5_Pin GPIO_PIN_5
#define D5_GPIO_Port GPIOD
#define D6_Pin GPIO_PIN_6
#define D6_GPIO_Port GPIOD
#define D7_Pin GPIO_PIN_7
#define D7_GPIO_Port GPIOD
#define IN0_Pin GPIO_PIN_8
#define IN0_GPIO_Port GPIOB
#define IN1_Pin GPIO_PIN_9
#define IN1_GPIO_Port GPIOB
#define FND1_CLK_Pin GPIO_PIN_0
#define FND1_CLK_GPIO_Port GPIOE
#define FND2_CLK_Pin GPIO_PIN_1
#define FND2_CLK_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
