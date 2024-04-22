/**
 * @file can_manager.c
 * @brief
 *
 * @date 15 Feb 2024
 * @author Giacomo Mazzucchi [giacomo.mazzucchi@protonmail.com]
 */

#include "can_manager.h"

enum can_mgr_errors {
  can_mgr_no_error,
  can_mgr_too_many_peripherals_error,
  can_mgr_hal_can_start_error,
  can_mgr_hal_activate_notification_error,
  can_mgr_hal_config_filter_error,
  can_mgr_hal_can_send_error,
  can_mgr_invalid_msg_index_error,
  can_mgr_index_out_of_bound_error,
  can_mgr_invalid_can_id_error,
  can_mgr_n_errors
};

CAN_HandleTypeDef *_can_mgr_peripherals[CAN_MGR_N_CAN];
int _can_mgr_fifo_assignment[CAN_MGR_TOTAL_CAN_RX_FIFOS];
can_mgr_msg_t *_can_mgr_msg_states[CAN_MGR_N_CAN];
uint8_t *_can_mgr_is_new_message[CAN_MGR_N_CAN];
int _can_mgr_msg_states_sizes[CAN_MGR_N_CAN];

int _can_mgr_current_can_counter = 0;

int can_mgr_error_code = can_mgr_no_error;
HAL_StatusTypeDef can_mgr_hal_code;

#define CAN_MGR_ID_CHECK(id)                                                   \
  if (id < 0 || id >= _can_mgr_current_can_counter) {                          \
    can_mgr_error_code = can_mgr_invalid_can_id_error;                         \
    return -1;                                                                 \
  }

int can_mgr_init(CAN_HandleTypeDef *hcan) {
  if (_can_mgr_current_can_counter == CAN_MGR_N_CAN) {
    can_mgr_error_code = can_mgr_too_many_peripherals_error;
    return -1;
  }
  _can_mgr_peripherals[_can_mgr_current_can_counter] = hcan;
  int assigned_id = _can_mgr_current_can_counter;
  _can_mgr_current_can_counter++;
  return assigned_id;
}

/**
 * Se message_states == NULL ignorare tutti i messaggi ricevuti (consigliato e'
 * farlo anche a livello hardware con i filtri)
 * Con NULL ci si aspetta state_size == 0
 */
int can_mgr_config(int can_id, CAN_FilterTypeDef *hfilter, uint32_t its, uint32_t rx_fifo_assignment, can_mgr_msg_t *message_states, uint8_t *message_is_new, size_t message_states_size) {
  CAN_MGR_ID_CHECK(can_id);
  _can_mgr_fifo_assignment[rx_fifo_assignment] = can_id;
  _can_mgr_msg_states[can_id] = message_states;
  _can_mgr_is_new_message[can_id] = message_is_new;
  _can_mgr_msg_states_sizes[can_id] = message_states_size;
#ifdef CAN_MGR_STM32_APPLICATION
  if (hfilter != NULL) {
    can_mgr_hal_code =
        HAL_CAN_ConfigFilter(_can_mgr_peripherals[can_id], hfilter);
    if (can_mgr_hal_code != HAL_OK) {
      can_mgr_error_code = can_mgr_hal_config_filter_error;
      return -1;
    }
  }
  can_mgr_error_code =
      HAL_CAN_ActivateNotification(_can_mgr_peripherals[can_id], its);
  if (can_mgr_hal_code != HAL_OK) {
    can_mgr_error_code = can_mgr_hal_activate_notification_error;
    return -1;
  }
#endif
  return 0;
}

int can_mgr_start(int can_id) {
  CAN_MGR_ID_CHECK(can_id);
#ifdef CAN_MGR_STM32_APPLICATION
  can_mgr_error_code = HAL_CAN_Start(_can_mgr_peripherals[can_id]);
  if (can_mgr_hal_code != HAL_OK) {
    can_mgr_error_code = can_mgr_hal_can_start_error;
    return -1;
  }
#endif
  return 0;
}

void _can_mgr_wait(CAN_HandleTypeDef *hcan) {
#ifdef CAN_MGR_STM32_APPLICATION
  uint32_t start_timestamp = HAL_GetTick();
  while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0)
    if (HAL_GetTick() > (start_timestamp + 5))
      return;
#endif
}

int can_mgr_send(int can_id, can_mgr_msg_t *msg) {
  CAN_MGR_ID_CHECK(can_id);
  // TODO: make this less hardware dependent (pass the header in the function call(?))
#ifdef CAN_MGR_STM32_APPLICATION
  CAN_HandleTypeDef *hcan = _can_mgr_peripherals[can_id];
  CAN_TxHeaderTypeDef header = {.StdId = msg->id,
                                .IDE = CAN_ID_STD,
                                .RTR = CAN_RTR_DATA,
                                .DLC = msg->size,
                                .TransmitGlobalTime = DISABLE};
#if CAN_MGR_CAN_WAIT_ENABLED == 1
  _can_mgr_wait(hcan);
#endif
  uint32_t mlb;
  can_mgr_hal_code = HAL_CAN_AddTxMessage(hcan, &header, msg->data, &mlb);
  if (can_mgr_hal_code != HAL_OK) {
    can_mgr_error_code = can_mgr_hal_can_send_error;
    return -1;
  }
#endif
  return 0;
}

// User-defined function
int can_mgr_from_id_to_index(int can_id, int msg_id);

void can_mgr_it_callback(CAN_HandleTypeDef *hcan, uint32_t rx_fifo_assignment, can_mgr_msg_t *mock_msg) {
  /**
   * TODO: integrate a priority queue to avoid O(n) search of incoming messages
  */ 
  int can_id = _can_mgr_fifo_assignment[rx_fifo_assignment];
  if (_can_mgr_msg_states[can_id] == NULL) {
    return;
  }
  int msg_id, msg_dlc;
  uint8_t msg_data[8] = {0};
#ifdef CAN_MGR_STM32_APPLICATION
  CAN_RxHeaderTypeDef header;
  HAL_CAN_GetRxMessage(hcan, rx_fifo_assignment, &header, msg_data);
  msg_id = header.StdId;
  msg_dlc = header.DLC;
#else
  msg_id = mock_msg->id;
  msg_dlc = mock_msg->size;
  memcpy(msg_data, mock_msg->data, msg_dlc);
#endif
  int index = can_mgr_from_id_to_index(can_id, msg_id);
  if (index < 0) {
    // ignore the message
  } else if (index >= _can_mgr_msg_states_sizes[can_id]) {
    can_mgr_error_code = can_mgr_index_out_of_bound_error;
  } else {
    _can_mgr_is_new_message[can_id][index] = 1;
    _can_mgr_msg_states[can_id][index].id = msg_id;
    _can_mgr_msg_states[can_id][index].size = msg_dlc;
    memcpy(_can_mgr_msg_states[can_id][index].data, msg_data, msg_dlc);
  }
}

