/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "time.h"
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

uint8_t can_msg_data[8];

/* CAN programming parameters */
const uint16_t PROG_MSG_ID = 0x101;
const uint32_t PROG_FREQ_HZ = 1;
const uint32_t PROG_TIME_SEC = 10;
const uint8_t PROG_CONSTANT_MSB = 0x75;
const uint8_t PROG_CONSTANT_LSB = 0x30;
const uint8_t NEW_BASE_ID_MSB = 0x01;
const uint8_t NEW_BASE_ID_LSB = 0x01;

/* Constants for IMU programming */
const uint8_t IMU_GYRO_245DPS = 1;
const uint8_t IMU_GYRO_500DPS = 2;
const uint8_t IMU_GYRO_2000DPS = 3;

const uint8_t IMU_ACCEL_2G = 1;
const uint8_t IMU_ACCEL_4G = 2;
const uint8_t IMU_ACCEL_8G = 3;
const uint8_t IMU_ACCEL_16G = 4;

const uint8_t IMU_FREQ_10HZ = 1;
const uint8_t IMU_FREQ_50HZ = 2;
const uint8_t IMU_FREQ_120HZ = 3;
const uint8_t IMU_FREQ_240HZ = 4;
const uint8_t IMU_FREQ_480HZ = 5;

const uint8_t IMU_BITRATE_1MBPS = 1;
const uint8_t IMU_BITRATE_500KBPS = 2;
const uint8_t IMU_BITRATE_250KBPS = 3;
const uint8_t IMU_BITRATE_100KBPS = 4;

/* Constants for IMU programming */
const uint8_t IRTS_EMISSIVITY = 30; /* 1..100 = 0.01..1.00 */

const uint8_t IRTS_FREQ_1HZ = 1;
const uint8_t IRTS_FREQ_2HZ = 2;
const uint8_t IRTS_FREQ_4HZ = 3;
const uint8_t IRTS_FREQ_8HZ = 4;
const uint8_t IRTS_FREQ_16HZ = 5;
const uint8_t IRTS_FREQ_32HZ = 6;
const uint8_t IRTS_FREQ_64HZ = 7;
const uint8_t IRTS_FREQ_100HZ = 8;

const uint8_t IRTS_CHANNELS_4 = 40;
const uint8_t IRTS_CHANNELS_8 = 80;
const uint8_t IRTS_CHANNELS_16 = 160;

const uint8_t IRTS_LAST_BYTE = 0x00;

/* Define the programming message here */
uint8_t can_msg_data[8] = {
  PROG_CONSTANT_MSB,
  PROG_CONSTANT_LSB,
  NEW_BASE_ID_MSB,
  NEW_BASE_ID_LSB,

  IRTS_EMISSIVITY,
  IRTS_FREQ_2HZ,
  IRTS_CHANNELS_4,
  IRTS_LAST_BYTE
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void serial_print(char * txt) {
    char buf[80] = "[MAIN] ";
    strncat(buf, txt, 80-strlen(buf)-1);
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), 500);
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
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();
  MX_USART3_UART_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  serial_print("Initialized\r\n");

  if (PROG_FREQ_HZ != 1) {
    serial_print("Programming is only supported at 1Hz frequency as of now.\r\n");
    return -1;
  }

  /* Print the packet contents to serial console */
  serial_print("CAN packet:\r\n");
  for (int i = 0; i < 8; i++) {
    char line[80];
    snprintf(line, 79, "  Byte %d: HEX 0x%02x (DEC %03d)\r\n", i+1, can_msg_data[i], can_msg_data[i]);
    serial_print(line);
  }

  /* Start the timer */
  HAL_TIM_Base_Start_IT(&htim6);

  while (1)
  {
    HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
    HAL_Delay(100);
    
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* Timer callback at 1Hz */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  static uint32_t elapsed_seconds = 0;

  if (elapsed_seconds < PROG_TIME_SEC) {
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, SET);
    can_send(PROG_MSG_ID, can_msg_data);
    elapsed_seconds++;
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, RESET);
   
    char msg[80];
    snprintf(msg, 79, "Programming: %ld%%\r\n", elapsed_seconds*100/PROG_TIME_SEC);
    serial_print(msg);
  }
  
  if (elapsed_seconds == PROG_TIME_SEC) {
    serial_print("Programming compelte. You may now unplug the\r\n");
    serial_print("sensor and reset it (disconnect power for >5 seconds).\r\n");
    elapsed_seconds++;
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 108;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV6;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
  PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
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
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
