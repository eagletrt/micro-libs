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

#ifndef CAN_MGR_H
#define CAN_MGR_H

#include "can_types.h"
#include "generic_queue.h"
#include <stdbool.h>

#include "main.h"

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
