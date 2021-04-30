#ifndef CAN_H
#define CAN_H

#include "stdint.h"

typedef struct {
    uint16_t id;
    uint16_t data;
} CAN_MessageTypeDef;

#endif