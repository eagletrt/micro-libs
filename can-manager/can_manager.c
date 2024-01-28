/****
 * Instruction to use the can manager library.
 * 
 * @author Giacomo Mazzucchi
 * @author Alessandro Conforti
 * @date 20th November 2023
 * 
 * Declare those extern variables to access the error codes.
 * 
 * extern HAL_StatusTypeDef can_manager_hal_status_retval;
 * extern int can_manager_error_code;
 * 
 * The can_manager_error_code is used in the init function 
 * to find which HAL_CAN function failed. 
 * The can_manager_hal_status_retval retrieves the status 
 * of the last failed HAL_CAN function.
 * 
 * This is a template for a standard CAN filter.
 * 
 * CAN_FilterTypeDef filter = {
 *  .FilterMode           = CAN_FILTERMODE_IDMASK,
 *  .FilterIdLow          = 0,
 *  .FilterIdHigh         = 0xFFFF,
 *  .FilterMaskIdHigh     = 0,
 *  .FilterMaskIdLow      = 0,
 *  .FilterFIFOAssignment = CAN_FILTER_FIFO0,
 *  .FilterBank           = 0,
 *  .FilterScale          = CAN_FILTERSCALE_16BIT,
 *  .FilterActivation     = ENABLE};
 * 
 * This is a template for a standard FDCAN filter.
 * 
 * FDCAN_FilterTypeDef filter = {
 *  .IdType           = FDCAN_STANDARD_ID,
 *  .FilterIndex      = 0,
 *  .FilterType       = FDCAN_FILTER_RANGE,
 *  .FilterConfig     = FDCAN_FILTER_TO_RXFIFO0,
 *  .FilterID1        = 0,
 *  .FilterID2        = ((1U << 11) - 1) << 8,
 *  .IsCalibrationMsg = 0,
 *  .RxBufferIndex    = 0};
 * 
 * This is instead a template for the standard CAN interrupt
 * 
 * CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_ERROR_WARNING | CAN_IT_ERROR_PASSIVE | CAN_IT_BUSOFF | CAN_IT_LAST_ERROR_CODE | CAN_IT_ERROR
 * 
 * with also the correct fifo assignment
 * 
 * CAN_IT_RX_FIFO0_MSG_PENDING
 * 
 * ++++++++++++++++++++++++
 * *** EXAMPLE CODE
 * ++++++++++++++++++++++++
 * 
 * int primary_can_id = -1;
 * int secondary_can_id = -1;
 * 
 * void f() {
 *  can_manager_message_t msg = {
 *      // compose your message
 *  };
 *  add_to_tx_queue(primary_can_id, &msg);
 * }
 * 
 * void handle_primary(can_manager_message_t* msg) {
 *  // your handler
 * }
 * 
 * // Example for message handler callback
 * void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
 *  CAN_RxHeaderTypeDef header = {};
 *  can_manager_message_t msg = {};
 *  HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &header, msg.data);
 *  msg.id = header.StdId;
 *  msg.size = header.DLC;
 *  add_to_rx_queue(primary_can_id, &msg)
 * }
 * 
 * int main() {
 * // hcan, handler, configs
 *  int primary_can_id = can_init(&hcan1, handle_primary, config, ...);
 *  int secondary_can_id = can_init(&hcan2, handle_secondary, config, ...);
 *  while(1) {
 *      consume_rx_queue(primary_can_id);
 *      consume_rx_queue(secondary_can_id);
 *      flush_tx_queue(primary_can_id);
 *      flush_tx_queue(secondary_can_id);
 *  }
 * }
 * 
*/

#include "can_manager.h"

int _n_active_can          = 0;
int can_manager_error_code = 0;
HAL_StatusTypeDef can_manager_hal_status_retval = HAL_OK;
generic_queue_t _rx_queues[CAN_MGR_MAX_CAN_BUSES];
generic_queue_t _tx_queues[CAN_MGR_MAX_CAN_BUSES];
uint8_t _rx_queues_data[CAN_MGR_MAX_QUEUE_ELEMENTS * sizeof(can_manager_message_t) * CAN_MGR_MAX_CAN_BUSES];
uint8_t _tx_queues_data[CAN_MGR_MAX_QUEUE_ELEMENTS * sizeof(can_manager_message_t) * CAN_MGR_MAX_CAN_BUSES];
void (*can_rx_msg_handlers[CAN_MGR_MAX_CAN_BUSES])(can_manager_message_t *);


#if FDCAN_MGR_ENABLED == 1

FDCAN_HandleTypeDef *fdcan_buses[MAX_CAN_BUSES];

int fdcan_init(
    FDCAN_HandleTypeDef *hcan, void(can_rx_msg_handler)(can_manager_message_t *), uint32_t activation_interrupt,
    FDCAN_FilterTypeDef *filter) {
    if (_n_active_can >= MAX_CAN_BUSES) {
        return -1;
    }

    int assigned_id                  = _n_active_can;
    can_rx_msg_handlers[assigned_id] = can_rx_msg_handler;

    GENQ_init(
        &_rx_queues[assigned_id], sizeof(can_manager_message_t), CAN_MGR_MAX_QUEUE_ELEMENTS,
        &_rx_queues_data[assigned_id]);
    GENQ_init(
        &_tx_queues[assigned_id], sizeof(can_manager_message_t), CAN_MGR_MAX_QUEUE_ELEMENTS,
        &_tx_queues_data[assigned_id]);

    fdcan_buses[assigned_id] = hcan;

    _n_active_can++;

    if ((can_manager_hal_status_retval = HAL_FDCAN_ConfigFilter(hcan, filter)) != HAL_OK) {
        can_manager_hal_status_retval = CAN_MGR_FILTER_ERROR_CODE;
        return -1;
    }
    if ((can_manager_hal_status_retval = HAL_FDCAN_ActivateNotification(hcan, activation_interrupt, 0)) != HAL_OK) {
        can_manager_hal_status_retval = CAN_MGR_CAN_INIT_IT_ERROR_CODE;
        return -1;
    }
    return assigned_id;
}

int fdcan_start(int can_id) {
    if ((can_manager_hal_status_retval = HAL_FDCAN_Start(fdcan_buses[can_id])) != HAL_OK) {
        can_manager_hal_status_retval = CAN_MGR_CAN_START_ERROR_CODE;
        return 0;
    }
    return 1;
}

void _fdcan_wait(FDCAN_HandleTypeDef *hcan) {
    uint32_t start_timestamp = HAL_GetTick();
    while (HAL_FDCAN_GetTxFifoFreeLevel(hcan) == 0) {
        if (HAL_GetTick() > start_timestamp + 5) {
            return;
        }
    }
}

int fdcan_send(int can_id, can_manager_message_t *msg) {
    CAN_MGR_ID_CHECK(can_id);
    FDCAN_HandleTypeDef *hcan = fdcan_buses[can_id];

    uint32_t dlc_len = 0;
    switch (msg->size) {
        case 0:
            dlc_len = FDCAN_DLC_BYTES_0;
            break;
        case 1:
            dlc_len = FDCAN_DLC_BYTES_1;
            break;
        case 2:
            dlc_len = FDCAN_DLC_BYTES_2;
            break;
        case 3:
            dlc_len = FDCAN_DLC_BYTES_3;
            break;
        case 4:
            dlc_len = FDCAN_DLC_BYTES_4;
            break;
        case 5:
            dlc_len = FDCAN_DLC_BYTES_5;
            break;
        case 6:
            dlc_len = FDCAN_DLC_BYTES_6;
            break;
        case 7:
            dlc_len = FDCAN_DLC_BYTES_7;
            break;
        case 8:
            dlc_len = FDCAN_DLC_BYTES_8;
            break;
    }

    FDCAN_TxHeaderTypeDef header = {
        .Identifier          = msg->id,
        .IdType              = FDCAN_STANDARD_ID,
        .TxFrameType         = FDCAN_DATA_FRAME,
        .DataLength          = dlc_len,
        .ErrorStateIndicator = FDCAN_ESI_ACTIVE,
        .BitRateSwitch       = FDCAN_BRS_OFF,
        .FDFormat            = FDCAN_CLASSIC_CAN,
        .TxEventFifoControl  = FDCAN_STORE_TX_EVENTS,
        .MessageMarker       = 0,
    };

#if CAN_WAIT_ENABLED == 1
    _fdcan_wait(hcan);
#endif

    if ((can_manager_hal_status_retval = HAL_FDCAN_AddMessageToTxFifoQ(hcan, &header, msg->data)) != HAL_OK) {
        return 0;
    }
    return 1;
}

#else

CAN_HandleTypeDef *can_buses[CAN_MGR_MAX_CAN_BUSES];

int can_init(
    CAN_HandleTypeDef *hcan, void(can_rx_msg_handler)(can_manager_message_t *), uint32_t activation_interrupt,
    CAN_FilterTypeDef *filter) {
    if (_n_active_can >= CAN_MGR_MAX_CAN_BUSES) {
        return -1;
    }

    int assigned_id                  = _n_active_can;
    can_rx_msg_handlers[assigned_id] = can_rx_msg_handler;

    GENQ_init(
        &_rx_queues[assigned_id], CAN_MGR_MAX_QUEUE_ELEMENTS * sizeof(can_manager_message_t),
        sizeof(can_manager_message_t), &_rx_queues_data[assigned_id]);
    GENQ_init(
        &_tx_queues[assigned_id], CAN_MGR_MAX_QUEUE_ELEMENTS * sizeof(can_manager_message_t),
        sizeof(can_manager_message_t), &_tx_queues_data[assigned_id]);

    can_buses[assigned_id] = hcan;
    _n_active_can++;

    if ((can_manager_hal_status_retval = HAL_CAN_ConfigFilter(hcan, filter)) != HAL_OK) {
        can_manager_error_code = CAN_MGR_FILTER_ERROR_CODE;
        return -1;
    }
    if ((can_manager_hal_status_retval = HAL_CAN_ActivateNotification(hcan, activation_interrupt)) != HAL_OK) {
        can_manager_error_code = CAN_MGR_CAN_INIT_IT_ERROR_CODE;
        return -1;
    }
    return assigned_id;
}

int can_start(int can_id) {
    if ((can_manager_hal_status_retval = HAL_CAN_Start(can_buses[can_id])) != HAL_OK) {
        can_manager_error_code = CAN_MGR_CAN_START_ERROR_CODE;
        return 0;
    }
    return 1;
}

void _can_wait(CAN_HandleTypeDef *hcan) {
    uint32_t start_timestamp = HAL_GetTick();
    while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0)
        if (HAL_GetTick() > start_timestamp + 5)
            return;
}

int can_send(int can_id, can_manager_message_t *msg) {
    CAN_MGR_ID_CHECK(can_id);
    CAN_HandleTypeDef *hcan = can_buses[can_id];
    CAN_TxHeaderTypeDef header = {
        .StdId = msg->id, .IDE = CAN_ID_STD, .RTR = CAN_RTR_DATA, .DLC = msg->size, .TransmitGlobalTime = DISABLE};

#if CAN_WAIT_ENABLED == 1
    _can_wait(hcan);
#endif
    uint32_t mlb = CAN_TX_MAILBOX0;
    if (HAL_CAN_AddTxMessage(hcan, &header, msg->data, &mlb) != HAL_OK) {
        return 0;
    }
    return 1;
}
#endif

int add_to_rx_queue(int can_id, can_manager_message_t *msg) {
    CAN_MGR_ID_CHECK(can_id);
    return GENQ_push(&_rx_queues[can_id], (uint8_t *)msg);
}

int add_to_tx_queue(int can_id, can_manager_message_t *msg) {
    CAN_MGR_ID_CHECK(can_id);
    return GENQ_push(&_tx_queues[can_id], (uint8_t *)msg);
}

int consume_rx_queue(int can_id) {
    CAN_MGR_ID_CHECK(can_id);
    can_manager_message_t msg;
    if (GENQ_pop(&_rx_queues[can_id], (uint8_t *)&msg)) {
        (*can_rx_msg_handlers[can_id])(&msg);
        return 1;
    }
    return 0;
}

int flush_tx_queue(int can_id) {
    CAN_MGR_ID_CHECK(can_id);
    can_manager_message_t msg;
    int sent_messages = 0;
    while (GENQ_pop(&_tx_queues[can_id], (uint8_t *)&msg)) {
#ifdef FDCAN_MGR
        if (!fdcan_send(can_id, &msg)) {
            return -1;
        }
#else
        if (!can_send(can_id, &msg)) {
            return -1;
        }
#endif
        ++sent_messages;
    }
    return sent_messages;
}
