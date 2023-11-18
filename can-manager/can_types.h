#ifndef CAN_TYPES_H
#define CAN_TYPES_H

#include <stdint.h>

typedef struct {
    uint16_t id;
    uint8_t size;
    uint8_t data[8];
} can_manager_message_t;

int consume_rx_queue(int can_id);
int flush_tx_queue(int can_id);
int add_to_rx_queue(int can_id, can_manager_message_t *msg);
int add_to_tx_queue(int can_id, can_manager_message_t *msg);

#endif  // #ifndef CAN_TYPES_H
