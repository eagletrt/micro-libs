#include "can.h"
#include "stdbool.h"
#include "../Lib/can-lib/lib/primary/primary_network.h"
#include "../Lib/can-lib/lib/secondary/secondary_network.h"

bool can_init_primary(CAN_HandleTypeDef *hcan, void(primary_can_rx_handler)(void));
bool can_init_secondary(CAN_HandleTypeDef *hcan, void(secondary_can_rx_handler)(void));

HAL_StatusTypeDef _CAN_activate_filter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *filter);
HAL_StatusTypeDef _CAN_activate_interrupts(CAN_HandleTypeDef *hcan);

bool primary_can_add_tx_message(CAN_MessageTypeDef message);
bool secondary_can_add_tx_message(CAN_MessageTypeDef message);

void consume_primary_rx_queue();
void consume_secondary_rx_queue();

bool primary_can_add_tx_message(CAN_MessageTypeDef *message);
bool secondary_can_add_tx_message(CAN_MessageTypeDef *message);

void primary_can_rx_handler(CAN_MessageTypeDef *message);
void secondary_can_rx_handler(CAN_MessageTypeDef *message);

void flush_tx_queue();

