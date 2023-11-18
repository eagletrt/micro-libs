/****
 * Instruction to use the can manager library.
 * 
 * @author Giacomo Mazzucchi
 * @author Alessandro Conforti
*/
#include "can_manager.h"

int _max_can_id = 0;
generic_queue_t _rx_queues[MAX_CAN_BUSES];
generic_queue_t _tx_queues[MAX_CAN_BUSES];
uint8_t _rx_queues_data[CAN_MANAGER_MAX_QUEUE_ELEMENTS * sizeof(generic_queue_t) * MAX_CAN_BUSES];
uint8_t _tx_queues_data[CAN_MANAGER_MAX_QUEUE_ELEMENTS * sizeof(generic_queue_t) * MAX_CAN_BUSES];
void (*can_rx_msg_handlers[MAX_CAN_BUSES])(can_manager_message_t *);

#ifdef FDCAN_MANAGER

FDCAN_HandleTypeDef *fdcan_buses[MAX_CAN_BUSES];

int fdcan_init(
    FDCAN_HandleTypeDef *hcan, void(can_rx_msg_handler)(can_manager_message_t *), uint32_t activation_interrupt,
    uint32_t filter_config) {
    if (_max_can_id >= MAX_CAN_BUSES) {
        return -1;
    }

    int assigned_id                  = _max_can_id;
    can_rx_msg_handlers[assigned_id] = can_rx_msg_handler;

    GENQ_init(
        &_rx_queues[assigned_id], sizeof(generic_queue_t), CAN_MANAGER_MAX_QUEUE_ELEMENTS,
        &_rx_queues_data[assigned_id]);
    GENQ_init(
        &_tx_queues[assigned_id], sizeof(generic_queue_t), CAN_MANAGER_MAX_QUEUE_ELEMENTS,
        &_tx_queues_data[assigned_id]);

    fdcan_buses[assigned_id] = hcan;

    _max_can_id++;

    FDCAN_FilterTypeDef filter = {
        .IdType           = FDCAN_STANDARD_ID,
        .FilterIndex      = 0,
        .FilterType       = FDCAN_FILTER_RANGE,
        .FilterConfig     = filter_config,
        .FilterID1        = 0,
        .FilterID2        = ((1U << 11) - 1) << 8,
        .IsCalibrationMsg = 0,
        .RxBufferIndex    = 0};

    // TODO better error handling
    HAL_FDCAN_ConfigFilter(hcan, &filter);
    HAL_FDCAN_ActivateNotification(hcan, activation_interrupt, 0);
    // TODO give the possibility to remove the start
    HAL_FDCAN_Start(hcan);

    return assigned_id;
}

void _fdcan_wait(FDCAN_HandleTypeDef *nwk) {
    uint32_t start_timestamp = HAL_GetTick();
    while (HAL_FDCAN_GetTxFifoFreeLevel(nwk) == 0)
        if (HAL_GetTick() > start_timestamp + 5)
            return;
}

int _fdcan_send(int can_id, can_manager_message_t *msg) {
    CM_CAN_ID_CHECK(can_id);
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

    // TODO flag for this
    _fdcan_wait(hcan);

    HAL_StatusTypeDef result = HAL_FDCAN_AddMessageToTxFifoQ(hcan, &header, msg->data);
    return result == HAL_OK;
}

#else

CAN_HandleTypeDef *can_buses[MAX_CAN_BUSES];

int can_init(
    CAN_HandleTypeDef *hcan, void(can_rx_msg_handler)(can_manager_message_t *), uint32_t activation_interrupt,
    uint32_t filter_config) {
    if (_max_can_id >= MAX_CAN_BUSES) {
        return -1;
    }

    int assigned_id                  = _max_can_id;
    can_rx_msg_handlers[assigned_id] = can_rx_msg_handler;

    GENQ_init(
        &_rx_queues[assigned_id], sizeof(generic_queue_t), CAN_MANAGER_MAX_QUEUE_ELEMENTS,
        &_rx_queues_data[assigned_id]);
    GENQ_init(
        &_tx_queues[assigned_id], sizeof(generic_queue_t), CAN_MANAGER_MAX_QUEUE_ELEMENTS,
        &_tx_queues_data[assigned_id]);

    can_buses[assigned_id] = hcan;
    _max_can_id++;

    CAN_FilterTypeDef filter =
    {.FilterMode           = CAN_FILTERMODE_IDMASK,
     .FilterIdLow          = 0,
     .FilterIdHigh         = 0xFFFF,
     .FilterMaskIdHigh     = 0,
     .FilterMaskIdLow      = 0,
     .FilterFIFOAssignment = filter_config,
     .FilterBank           = 0,
     .FilterScale          = CAN_FILTERSCALE_16BIT,
     .FilterActivation     = ENABLE }

    // TODO better error handling
    if (HAL_CAN_ConfigFilter(hcan, &filter) != HAL_OK) {
        return -1;
    }
    if (HAL_CAN_ActivateNotification(hcan, activation_interrupt) != HAL_OK) {
        return -1;
    }
    HAL_CAN_ActivateNotification(
        hcan, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_ERROR_WARNING | CAN_IT_ERROR_PASSIVE | CAN_IT_BUSOFF |
                  CAN_IT_LAST_ERROR_CODE | CAN_IT_ERROR);
    if (HAL_CAN_Start(hcan) != HAL_OK) {
        return -1;
    }
    return assigned_id;
}

void _can_wait(CAN_HandleTypeDef *hcan) {
    uint32_t start_timestamp = HAL_GetTick();
    while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0)
        if (HAL_GetTick() > start_timestamp + 5)
            return;
}

int _can_send(int can_id, can_manager_message_t *msg) {
    CM_CAN_ID_CHECK(can_id);
    CAN_HandleTypeDef *hcan = can_buses[can_id];
    CAN_TxHeaderTypeDef header =
        {.StdId = msg->id, .IDE = CAN_ID_STD, .RTR = CAN_RTR_DATA, .DLC = msg->size, .TransmitGlobalTime = DISABLE}

    // TODO flag for this
    _can_wait(hcan);

    if (HAL_CAN_AddTxMessage(hcan, &header, msg->data, NULL) != HAL_OK) {
        return 0;
    }
    return 1;
}
#endif

int add_to_rx_queue(int can_id, can_manager_message_t *msg) {
    CM_CAN_ID_CHECK(can_id);
    return GENQ_push(&_rx_queues[can_id], (uint8_t *)msg);
}

int add_to_tx_queue(int can_id, can_manager_message_t *msg) {
    CM_CAN_ID_CHECK(can_id);
    return GENQ_push(&_tx_queues[can_id], (uint8_t *)msg);
}

int consume_rx_queue(int can_id) {
    CM_CAN_ID_CHECK(can_id);
    can_manager_message_t msg;
    if (GENQ_pop(&_rx_queues[can_id], (uint8_t *)&msg)) {
        (*can_rx_msg_handlers[can_id])(&msg);
        return 1;
    }
    return 0;
}

int flush_tx_queue(int can_id) {
    CM_CAN_ID_CHECK(can_id);
    can_manager_message_t msg;
    if (GENQ_pop(&_tx_queues[can_id], (uint8_t *)&msg)) {
#ifdef FDCAN_MANAGER
        return _fdcan_send(can_id, &msg);
#else
        return _can_send(can_id, &msg);
#endif
    }
    return 0;
}
