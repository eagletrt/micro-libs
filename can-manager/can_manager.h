#ifndef CAN_MANAGER_H
#define CAN_MANAGER_H

#include "can_types.h"
#include "generic_queue.h"
#include "main.h"
#include "stdbool.h"

#define MAX_CAN_BUSES                  2
#define CAN_MANAGER_MAX_QUEUE_ELEMENTS 50

#define CM_CAN_ID_CHECK(can_id)                \
    if (can_id < 0 || can_id >= MAX_CAN_BUSES) \
    return 0

#ifdef FDCAN_MANAGER
int fdcan_init(
    FDCAN_HandleTypeDef *hcan, void(can_rx_msg_handler)(can_manager_message_t *), uint32_t activation_interrupt,
    uint32_t filter_config);
#else
int can_init(
    CAN_HandleTypeDef *hcan, void(can_rx_msg_handler)(can_manager_message_t *), uint32_t activation_interrupt,
    uint32_t filter_config);
#endif

#endif  // CAN_MANAGER_H

/***
 * 

// CAN_interrupt
can_manager_add_rx_queue(primary_can_id, &can_msg);

int main() {
    // hcan, handler, priority
    int primary_can_id = can_init(&hcan1, handle_primary);
    int secondary_can_id = can_init(&hcan2, handle_secondary);

    while(1) {
        consume_rx_queue(primary_can_id);
        consume_rx_queue(secondary_can_id);
        flush_tx_queue(primary_can_id);
        flush_tx_queue(secondary_can_id);
    }
}

// libreria
void consume_rx_queue() {
    // una volta per ogni coda, bastano circular_buffer
}

*/
