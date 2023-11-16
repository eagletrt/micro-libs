#include "can-manager.h"

float CAN_error_rate;
CANFQ_QueueTypeDef primary_rx_queue;
CANFQ_QueueTypeDef secondary_rx_queue;
CANFQ_QueueTypeDef primary_tx_queue;
CANFQ_QueueTypeDef secondary_tx_queue;
bool readPrimary = true;
CAN_HandleTypeDef *_can_primary;
CAN_HandleTypeDef *_can_secondary;

can_init_primary(CAN_HandleTypeDef * hcan, void(primary_can_rx_handler)(void)) {
    _can_primary = hcan;

    CAN_FilterTypeDef filter;

    filter->FilterMode = CAN_FILTERMODE_IDMASK;
    filter->FilterIdLow = 0;
    filter->FilterIdHigh = 0xFFFF;
    filter->FilterMaskIdHigh = 0;
    filter->FilterMaskIdLow = 0;
    filter->FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter->FilterBank = 0;
    filter->FilterScale = CAN_FILTERSCALE_16BIT;
    filter->FilterActivation = ENABLE;

    _CAN_activate_filter(hcan, &filter);

    if (HAL_CAN_Start(hcan) != HAL_OK)
        _CAN_error_handler("HAL_CAN_Start() failed");

    return true;
}

bool can_init_secondary(CAN_HandleTypeDef * hcan, void(secondary_can_rx_handler)(void)) {
    _can_secondary = hcan;

    CAN_FilterTypeDef filter;

    filter->FilterMode = CAN_FILTERMODE_IDMASK;
    filter->FilterIdLow = 0;
    filter->FilterIdHigh = 0xFFFF;
    filter->FilterMaskIdHigh = 0;
    filter->FilterMaskIdLow = 0;
    filter->FilterFIFOAssignment = CAN_FILTER_FIFO1;
    filter->FilterBank = 1;
    filter->FilterScale = CAN_FILTERSCALE_16BIT;
    filter->FilterActivation = ENABLE;
    filter->SlaveStartFilterBank = 1;

    _CAN_activate_filter(hcan, &filter);

    if (HAL_CAN_Start(hcan) != HAL_OK)
        _CAN_error_handler("HAL_CAN_Start() failed");

    return true;
}

HAL_StatusTypeDef _CAN_activate_filter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filter) {
    HAL_StatusTypeDef s;
    if ((s = HAL_CAN_ConfigFilter(hcan, filter)) != HAL_OK)
        _CAN_error_handler("Failed to initialize a CAN filter");
    
    return s;
}

bool primary_can_add_tx_message(CAN_MessageTypeDef message) {
    return CANFQ_push(primary_rx_queue, message);
}

bool secondary_can_add_tx_message(CAN_MessageTypeDef message) {
    return CANFQ_push(secondary_rx_queue, message);
}

HAL_StatusTypeDef _CAN_activate_interrupts(CAN_HandleTypeDef *hcan) {
    HAL_StatusTypeDef s_rx;

    //TODO: eh?
    // uint32_t irq_rx = (hcan == &CAN_PRIMARY_NETWORK) ?
    //     CAN_IT_RX_FIFO0_MSG_PENDING : CAN_IT_RX_FIFO1_MSG_PENDING;

    // if ((s_rx = HAL_CAN_ActivateNotification(hcan, irq_rx)) != HAL_OK)
    //     _CAN_error_handler("Failed to activate a CAN RX interrupt");
    
    HAL_CAN_ActivateNotification(hcan,
        CAN_IT_TX_MAILBOX_EMPTY |
        CAN_IT_ERROR_WARNING |
        CAN_IT_ERROR_PASSIVE |
        CAN_IT_BUSOFF |
        CAN_IT_LAST_ERROR_CODE |
        CAN_IT_ERROR
    );
    
    return s_rx;
}

void _CAN_error_handler(char * msg) {
    _CAN_err_count++;
    CAN_error_rate = (float)_CAN_err_count / (float)_CAN_tot_tx_count;
    
    #if CAN_DEBUG
        LOG_write(LOGLEVEL_ERR, "[CAN/Error Handler] %s", msg);
    #endif
}

void consume_primary_rx_queue() {
    CAN_MessageTypeDef message;
    CANFQ_pop(&primary_rx_queue, &message);
    primary_can_rx_handler(&message);
}

void consume_secondary_rx_queue() {
    CAN_MessageTypeDef message;
    CANFQ_pop(&secondary_rx_queue, &message);
    secondary_can_rx_handler(&message);
}

bool primary_can_add_tx_message(CAN_MessageTypeDef *message) {
    CANFQ_push(&primary_tx_queue, message);
    return false;
}

bool secondary_can_add_tx_message(CAN_MessageTypeDef *message) {
    CANFQ_push(&secondary_tx_queue, message);
    return false;
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
    uint32_t e = hcan->ErrorCode;

    if (e & HAL_CAN_ERROR_EWG)
        _CAN_error_handler("Protocol Error Warning");
    if (e & HAL_CAN_ERROR_EPV)
        _CAN_error_handler("Error Passive");
    if (e & HAL_CAN_ERROR_BOF)
        _CAN_error_handler("Bus-off Error");
    if (e & HAL_CAN_ERROR_STF)
        _CAN_error_handler("Stuff Error");
    if (e & HAL_CAN_ERROR_FOR)
        _CAN_error_handler("Form Error");
    if (e & HAL_CAN_ERROR_ACK)
        _CAN_error_handler("ACK Error");
    if (e & HAL_CAN_ERROR_BR)
        _CAN_error_handler("Bit Recessive Error");
    if (e & HAL_CAN_ERROR_BD)
        _CAN_error_handler("Bit Dominant Error");
    if (e & HAL_CAN_ERROR_CRC)
        _CAN_error_handler("CRC Error");
    if (e & HAL_CAN_ERROR_RX_FOV0)
        _CAN_error_handler("FIFO0 Overrun");
    if (e & HAL_CAN_ERROR_RX_FOV1)
        _CAN_error_handler("FIFO1 Overrun");
    if (e & HAL_CAN_ERROR_TX_ALST0)
        _CAN_error_handler("Mailbox 0 TX failure (arbitration lost)");
    if (e & HAL_CAN_ERROR_TX_TERR0)
        _CAN_error_handler("Mailbox 0 TX failure (tx error)");
    if (e & HAL_CAN_ERROR_TX_ALST1)
        _CAN_error_handler("Mailbox 1 TX failure (arbitration lost)");
    if (e & HAL_CAN_ERROR_TX_TERR1)
        _CAN_error_handler("Mailbox 1 TX failure (tx error)");
    if (e & HAL_CAN_ERROR_TX_ALST2)
        _CAN_error_handler("Mailbox 2 TX failure (arbitration lost)");
    if (e & HAL_CAN_ERROR_TX_TERR2)
        _CAN_error_handler("Mailbox 2 TX failure (tx error)");
    if (e & HAL_CAN_ERROR_TIMEOUT)
        _CAN_error_handler("Timeout Error");
    if (e & HAL_CAN_ERROR_NOT_INITIALIZED)
        _CAN_error_handler("Peripheral not initialized");
    if (e & HAL_CAN_ERROR_NOT_READY)
        _CAN_error_handler("Peripheral not ready");
    if (e & HAL_CAN_ERROR_NOT_STARTED)
        _CAN_error_handler("Peripheral not strated");
    if (e & HAL_CAN_ERROR_PARAM)
        _CAN_error_handler("Parameter Error");
}

HAL_StatusTypeDef CAN_send(CAN_MessageTypeDef *message, CAN_HandleTypeDef *hcan) {
    CAN_TxHeaderTypeDef header;
    header.StdId = message->id;
    header.IDE = CAN_ID_STD;
    header.RTR = CAN_RTR_DATA;
    header.DLC = message->size;
    header.TransmitGlobalTime = DISABLE;

    _CAN_wait(hcan);

    //TODO add _inc_tot_tx() function
    _CAN_tot_tx_count++;
    CAN_error_rate = (float)_CAN_err_count / (float)_CAN_tot_tx_count;

    #if CAN_DEBUG
      LOG_write(LOGLEVEL_DEBUG, "[CAN] Sending 0x%02X", message->id);
    #endif

    return HAL_CAN_AddTxMessage(hcan, &header, message->data, NULL);
}

void flush_tx_queue() {
    CAN_MessageTypeDef message;
    while (!CANFQ_is_empty(*primary_tx_queue)) {
        CANFQ_pop(&primary_rx_queue, &message);
        CAN_send(&message, _can_primary);
    }
    while (!CANFQ_is_empty(*secondary_tx_queue)) {
        CANFQ_pop(&secondary_rx_queue, &message);
        CAN_send(&message, _can_secondary);
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    _CAN_process_incoming_rx(hcan, CAN_RX_FIFO0);
    CAN_RxHeaderTypeDef header = {};
	CAN_MessageTypeDef msg = {};
    HAL_CAN_GetRxMessage(hcan, rx_fifo, &header, msg.data);
	msg.id = header.StdId;
	msg.size = header.DLC;
    msg.hcan = hcan;
}
