#include "../can_manager.h"

void consume_primary_rx_queue(can_manager_message_t *msg) {
    printf("RECEIVED can: primary, ");
    print_message(msg);
}

void consume_secondary_rx_queue(can_manager_message_t *msg) {
    printf("RECEIVED can: secondary, ");
    print_message(msg);
}

int main() {
    CAN_HandleTypeDef primary_can_handle;
    CAN_HandleTypeDef secondary_can_handle;
    primary_can_handle.name   = "primary";
    secondary_can_handle.name = "secondary";

    int can_primary_id   = can_init(&primary_can_handle, consume_primary_rx_queue, 0, 0);
    int can_secondary_id = can_init(&secondary_can_handle, consume_secondary_rx_queue, 0, 0);

    can_manager_message_t msg1 = {.id = 1, .size = 8, .data = {4, 6, 2, 8, 2, 4, 8, 6}};
    can_manager_message_t msg2 = {.id = 2, .size = 3, .data = {9, 1, 0, 0, 3, 2, 5, 4}};
    can_manager_message_t msg3 = {.id = 3, .size = 7, .data = {8, 2, 3, 4, 5, 6, 7, 1}};
    can_manager_message_t msg4 = {.id = 4, .size = 5, .data = {3, 7, 3, 6, 9, 5, 8, 5}};

    add_to_tx_queue(can_primary_id, &msg1);
    add_to_rx_queue(can_secondary_id, &msg2);
    add_to_tx_queue(can_primary_id, &msg3);
    add_to_rx_queue(can_secondary_id, &msg4);

    while (consume_rx_queue(can_primary_id))
        ;

    printf("sent %d messages primary\n", flush_tx_queue(can_primary_id));

    while (consume_rx_queue(can_secondary_id))
        ;

    printf("sent %d messages secondary\n", flush_tx_queue(can_secondary_id));

    return 0;
}