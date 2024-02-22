/**
 * @file can_manager_can_types.h
 * @brief Hardware independent header file
 *
 * @date 15 Feb 2024
 * @author Giacomo Mazzucchi [giacomo.mazzucchi@protonmail.com]
 */

#ifndef CAN_MANAGER_CANTYPES_H
#define CAN_MANAGER_CANTYPES_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint16_t id;
  uint8_t size;
  uint8_t data[8];
} can_mgr_msg_t;

int can_mgr_send(int can_id, can_mgr_msg_t *msg);
can_mgr_msg_t **get_can_mgr_states(void);
int can_mgr_start(int can_id);

#endif // CAN_MANAGER_CANTYPES_H
