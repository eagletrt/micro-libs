#include "invlib.h"


int32_t _INVLIB_max_rpm = 0;


void INVLIB_set_max_rpm(int32_t v) {
    _INVLIB_max_rpm = v;
}


int32_t INVLIB_parse_rpm(uint8_t byte_1, uint8_t byte_2) {
    int16_t raw_val = ((byte_2 << 8) | byte_2);
    
    /* Note: 32767 is 2^15-1, aka the max positive value of the int16 param */
    return _INVLIB_max_rpm * raw_val / 32767;
}
