#include "circ-buf.h"


#ifdef NOLOGGER
    #include <stdio.h>
    #define LOG_write(x, y) printf(y)
#else
    #include "logger.h"
#endif


void CBUF_init(CBUF_HandleTypeDef *cb) {
    cb->start = 0;
    cb->end = 0;
}

uint16_t CBUF_enqueue(CBUF_HandleTypeDef *cb, uint8_t item) {
    if (CBUF_is_full(cb)) {
        LOG_write(LOGLEVEL_ERR, "[CBUF] Buffer overrun, discarding byte");
    } else {
        cb->buf[cb->end++] = item;
        cb->end %= CBUF_SIZE;
    }

    return cb->end;
}

uint8_t CBUF_dequeue(CBUF_HandleTypeDef *cb) {
    if (CBUF_is_empty(cb)) {
        LOG_write(LOGLEVEL_WARN, "[CBUF] Buffer is empty, returning 0x00");
        return 0U;
    } else {
        uint8_t data = cb->buf[cb->start++];
        cb->start %= CBUF_SIZE;
        return data;
    }
}

bool CBUF_is_empty(CBUF_HandleTypeDef *cb) {
    return cb->start == cb->end;
}

bool CBUF_is_full(CBUF_HandleTypeDef *cb) {
    return (cb->end + 1) % CBUF_SIZE == cb->start;
}