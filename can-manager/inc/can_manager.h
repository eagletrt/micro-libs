/**
 * @file can_manager.h
 * @brief It is recommended to include this header file only in can.c (in a
 * STM32 CubeMX based project)
 *
 * @date 15 Feb 2024
 * @author Giacomo Mazzucchi [giacomo.mazzucchi@protonmail.com]
 */

#ifndef CAN_MANAGER_H
#define CAN_MANAGER_H

#include "can_manager_can_types.h"
#include "can_manager_config.h"

#ifndef CAN_MGR_STM32_APPLICATION
#include "can_manager_type_mocking.h"
#else
#include "main.h"
#endif

extern can_mgr_msg_t *can_mgr_msg_states[CAN_MGR_N_CAN];
extern int can_mgr_error_code;
extern HAL_StatusTypeDef can_mgr_hal_code;

int can_mgr_init(CAN_HandleTypeDef *hcan);
int can_mgr_config(int can_id, CAN_FilterTypeDef *hfilter, uint32_t its, uint32_t rx_fifo_assignment, can_mgr_msg_t *message_states, size_t message_states_size);
int can_mgr_start(int can_id);

#endif // CAN_MANAGER_H
