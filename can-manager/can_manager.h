#ifndef CAN_MGR_H
#define CAN_MGR_H

#include "can_types.h"
#include "generic_queue.h"
#include "main.h"
#include "stdbool.h"

#define CAN_MGR_MAX_CAN_BUSES          2
#define CAN_MGR_MAX_QUEUE_ELEMENTS     10
#define CAN_MGR_CAN_WAIT_ENABLED       1
#define CAN_MGR_FILTER_ERROR_CODE      2
#define CAN_MGR_CAN_INIT_IT_ERROR_CODE 3
#define CAN_MGR_CAN_START_ERROR_CODE   4

#define CAN_MGR_ID_CHECK(can_id)                       \
    if (can_id < 0 || can_id >= CAN_MGR_MAX_CAN_BUSES) \
    return 0

#ifdef FDCAN_MGR
int fdcan_init(
    FDCAN_HandleTypeDef *hcan, void(can_rx_msg_handler)(can_manager_message_t *), uint32_t activation_interrupt,
    FDCAN_FilterTypeDef *filter);
#else
int can_init(
    CAN_HandleTypeDef *hcan, void(can_rx_msg_handler)(can_manager_message_t *), uint32_t activation_interrupt,
    CAN_FilterTypeDef *filter);
#endif

#endif  // CAN_MGR_H
