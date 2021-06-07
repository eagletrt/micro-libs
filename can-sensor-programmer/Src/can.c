/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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

/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include "stdbool.h"
#include "inttypes.h"
#include "string.h"
#include "stdio.h"
#include "can.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_can.h"
#include "usart.h"


/**
 * @brief Print messages over the serial UART, prefixed with "[CAN]"
 * */
void CAN_serial_print(char * txt) {
    char buf[80] = "[CAN] ";
    strncat(buf, txt, 80-strlen(buf)-1);
    HAL_UART_Transmit(&huart3, (uint8_t *)buf, strlen(buf), 500);
}

/**
 * @brief Print the error message in the serial console and activate
 *        the CAN error LED
 * @param msg The message to send over UART
 * */
void CAN_error_handler(char * msg) {
  CAN_serial_print(msg);

  uint32_t err_code = HAL_CAN_GetError(&hcan1);

  if (err_code != HAL_CAN_ERROR_NONE)
    CAN_serial_print("Error bits set:\r\n");
  if ((err_code & HAL_CAN_ERROR_EWG) == HAL_CAN_ERROR_EWG)
    CAN_serial_print("  + Protocol error warning\r\n");
  if ((err_code & HAL_CAN_ERROR_EPV) == HAL_CAN_ERROR_EPV)
    CAN_serial_print("  + Error passive\r\n");
  if ((err_code & HAL_CAN_ERROR_BOF) == HAL_CAN_ERROR_BOF)
    CAN_serial_print("  + Bus-off error\r\n");
  if ((err_code & HAL_CAN_ERROR_STF) == HAL_CAN_ERROR_STF)
    CAN_serial_print("  + Stuff error\r\n");
  if ((err_code & HAL_CAN_ERROR_FOR) == HAL_CAN_ERROR_FOR)
    CAN_serial_print("  + Form error\r\n");
  if ((err_code & HAL_CAN_ERROR_ACK) == HAL_CAN_ERROR_ACK)
    CAN_serial_print("  + ACK error\r\n");
  if ((err_code & HAL_CAN_ERROR_BR) == HAL_CAN_ERROR_BR)
    CAN_serial_print("  + Bit Recessive error\r\n");
  if ((err_code & HAL_CAN_ERROR_BD) == HAL_CAN_ERROR_BD)
    CAN_serial_print("  + Bit Dominant error\r\n");
  if ((err_code & HAL_CAN_ERROR_CRC) == HAL_CAN_ERROR_CRC)
    CAN_serial_print("  + CRC error\r\n");
  if ((err_code & HAL_CAN_ERROR_RX_FOV0) == HAL_CAN_ERROR_RX_FOV0)
    CAN_serial_print("  + FIFO 0 overrun error\r\n");
  if ((err_code & HAL_CAN_ERROR_RX_FOV1) == HAL_CAN_ERROR_RX_FOV1)
    CAN_serial_print("  + FIFO 1 overrun error\r\n");
  if ((err_code & HAL_CAN_ERROR_TX_ALST0) == HAL_CAN_ERROR_TX_ALST0)
    CAN_serial_print("  + TX 0 arbitration lost error\r\n");
  if ((err_code & HAL_CAN_ERROR_TX_TERR0) == HAL_CAN_ERROR_TX_TERR0)
    CAN_serial_print("  + TX 0 transmit error\r\n");
  if ((err_code & HAL_CAN_ERROR_TX_ALST1) == HAL_CAN_ERROR_TX_ALST1)
    CAN_serial_print("  + TX 1 arbitration lost error\r\n");
  if ((err_code & HAL_CAN_ERROR_TX_TERR1) == HAL_CAN_ERROR_TX_TERR1)
    CAN_serial_print("  + TX 1 transmit error\r\n");
  if ((err_code & HAL_CAN_ERROR_TX_ALST2) == HAL_CAN_ERROR_TX_ALST2)
    CAN_serial_print("  + TX 2 arbitration lost error\r\n");
  if ((err_code & HAL_CAN_ERROR_TX_TERR2) == HAL_CAN_ERROR_TX_TERR2)
    CAN_serial_print("  + TX 2 transmit error\r\n");
  if ((err_code & HAL_CAN_ERROR_TIMEOUT) == HAL_CAN_ERROR_TIMEOUT)
    CAN_serial_print("  + Timeout error\r\n");
  if ((err_code & HAL_CAN_ERROR_NOT_INITIALIZED) == HAL_CAN_ERROR_NOT_INITIALIZED)
    CAN_serial_print("  + CAN bus not initialized\r\n");
  if ((err_code & HAL_CAN_ERROR_NOT_READY) == HAL_CAN_ERROR_NOT_READY)
    CAN_serial_print("  + CAN bus not ready\r\n");
  if ((err_code & HAL_CAN_ERROR_NOT_STARTED) == HAL_CAN_ERROR_NOT_STARTED)
    CAN_serial_print("  + CAN bus not started\r\n");
  if ((err_code & HAL_CAN_ERROR_PARAM) == HAL_CAN_ERROR_PARAM)
    CAN_serial_print("  + Parameter error\r\n");
  if ((err_code & HAL_CAN_ERROR_INTERNAL) == HAL_CAN_ERROR_INTERNAL)
    CAN_serial_print("  + Internal error\r\n");
  
  uint8_t rec_val = 0;
  uint8_t tec_val = 0;
  rec_val = (uint8_t)((hcan1.Instance->ESR >> 24) && 0xFFU);
  tec_val = (uint8_t)((hcan1.Instance->ESR >> 16) && 0xFFU);
  char counters_status[40];
  sprintf(counters_status, "Value of REC %d, Value of TEC %d \r\n", rec_val, tec_val);
  CAN_serial_print(counters_status);
}

/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 1;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_15TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  // Togliendo il filtro non riceve nemmeno in loopback
  CAN_FilterTypeDef f;
  f.FilterMode = CAN_FILTERMODE_IDMASK;
  f.FilterIdLow = 0;
  f.FilterIdHigh = 0xFFFF;
  f.FilterMaskIdHigh = 0;
  f.FilterMaskIdLow = 0;
  f.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  f.FilterBank = 0;
  f.FilterScale = CAN_FILTERSCALE_16BIT;
  f.FilterActivation = ENABLE;
  
  if (HAL_CAN_ConfigFilter(&hcan1, &f) != HAL_OK)
    CAN_error_handler("Failed to initialize CAN filter");

  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK)
    CAN_error_handler("Failed to activate a CAN RX interrupts\r\n");

  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_ERROR) != HAL_OK)
      CAN_error_handler("Failed to activate a CAN SCE interrupt\r\n");

  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_ERROR_WARNING) != HAL_OK)
      CAN_error_handler("Failed to activate a CAN SCE interrupt\r\n");

  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_ERROR_PASSIVE) != HAL_OK)
      CAN_error_handler("Failed to activate a CAN SCE interrupt\r\n");

  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_BUSOFF) != HAL_OK)
      CAN_error_handler("Failed to activate a CAN SCE interrupt\r\n");

  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_LAST_ERROR_CODE) != HAL_OK)
      CAN_error_handler("Failed to activate a CAN SCE interrupt\r\n");

  if (HAL_CAN_Start(&hcan1) != HAL_OK)
    CAN_error_handler("CAN_Start failed\r\n");
  /* USER CODE END CAN1_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PD0     ------> CAN1_RX
    PD1     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_1);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX1_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_SCE_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */


void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
  char txt[50];
  sprintf(txt, "Error! Code: %lu\r\n", (uint32_t)hcan->ErrorCode);
  CAN_error_handler(txt);
}

void _can_handle_rx(CAN_HandleTypeDef *hcan, uint32_t fifo) {
  CAN_RxHeaderTypeDef h;
  uint8_t data[8];
  HAL_CAN_GetRxMessage(&hcan1, fifo, &h, data);

  char txt[80], *offset = txt;
  offset += snprintf(txt, 79, "RX [ID: 0x%lx]", h.StdId);
  for (int i = 0; i < h.DLC; i++)
    offset += sprintf(offset, " %03d", data[i]);
  sprintf(offset, "%s", "\r");
  // CAN_serial_print(txt);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  _can_handle_rx(hcan, CAN_RX_FIFO0);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
  _can_handle_rx(hcan, CAN_RX_FIFO1);
}

void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan) {
	CAN_error_handler("RX FIFO#0 is full");
}
void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan) {
	CAN_error_handler("RX FIFO#1 is full");
}
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
	CAN_serial_print("Mailbox complete\r\n");
}
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) {
	CAN_serial_print("Mailbox complete\r\n");
}
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) {
	CAN_serial_print("Mailbox complete\r\n");
}


HAL_StatusTypeDef can_send(uint16_t msg_id, uint8_t* data) {
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, SET);

  if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    CAN_error_handler("No free mailboxes available\r\n");
    return HAL_ERROR;
  }

  CAN_TxHeaderTypeDef header;
  HAL_StatusTypeDef status;
  uint32_t free_mailbox;

  /* Prepare TX header */
  header.StdId = msg_id;
  header.IDE = CAN_ID_STD;
  header.DLC = 8;
  header.RTR = CAN_RTR_DATA;
  header.TransmitGlobalTime = DISABLE;

  /* Send the message */
  status = HAL_CAN_AddTxMessage(&hcan1, &header, data, &free_mailbox);
  if (status != HAL_OK)
    CAN_error_handler("AddTxMessage failed\r\n");
  
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, RESET);
  return status;
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
