#ifndef CTRL_NWK_UTILS_H
#define CTRL_NWK_UTILS_H

#include "stdint.h"
#include "stdbool.h"


typedef struct {
    uint8_t ParamID;
    float ParamVal;
    uint16_t CRC;
} CTRL_PayloadTypeDef;


bool CTRL_read_frame(uint8_t*, uint8_t, CTRL_PayloadTypeDef*);

uint8_t CTRL_unstuff_buffer(uint8_t*, uint8_t, uint8_t*);

uint8_t CTRL_stuff_buffer(uint8_t*, uint8_t, uint8_t*);

uint8_t CTRL_compose_frame(CTRL_PayloadTypeDef*, uint8_t*);

#endif