#ifndef CAN_TYPES_H
#define CAN_TYPES_H

#include <stdint.h>

#define CAN_MGR_MAX_CAN_BUSES          2
#define CAN_MGR_MAX_QUEUE_ELEMENTS     10
#define CAN_MGR_CAN_WAIT_ENABLED       1
#define CAN_MGR_FILTER_ERROR_CODE      2
#define CAN_MGR_CAN_INIT_IT_ERROR_CODE 3
#define CAN_MGR_CAN_START_ERROR_CODE   4

typedef struct {
    uint16_t id;
    uint8_t size;
    uint8_t data[8];
} can_manager_message_t;

#ifdef FDCAN_MGR
int fdcan_send(int can_id, can_manager_message_t *msg);
int fdcan_start(int can_id);
#else
int can_send(int can_id, can_manager_message_t *msg);
int can_start(int can_id);
#endif
int consume_rx_queue(int can_id);
int flush_tx_queue(int can_id);
int add_to_rx_queue(int can_id, can_manager_message_t *msg);
int add_to_tx_queue(int can_id, can_manager_message_t *msg);

#endif  // #ifndef CAN_TYPES_H
