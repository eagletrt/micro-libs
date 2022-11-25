#ifndef CIRC_BUF_H
#define CIRC_BUF_H

#include "stdint.h"
#include "stdbool.h"

#define CBUF_SIZE 255


typedef struct {
    uint16_t start, end;
    uint16_t buf[CBUF_SIZE];
} CBUF_HandleTypeDef;


void CBUF_init(CBUF_HandleTypeDef *cb);
void CBUF_enqueue(CBUF_HandleTypeDef *cb, uint8_t item);
uint8_t CBUF_dequeue(CBUF_HandleTypeDef *cb);
bool CBUF_is_full(CBUF_HandleTypeDef *cb);
bool CBUF_is_empty(CBUF_HandleTypeDef *cb);

#endif