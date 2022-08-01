#ifndef INVLIB_H
#define INVLIB_H

#include "stdint.h"

/**
 * @brief     Set the maximum RPM at which the motors will be driven
 * @param     nmax The Nmax parameter from NDrive, needed to parse the RPM raw value
 */
void INVLIB_set_max_rpm(int32_t nmax);


/**
 * @brief     Parse the raw RPM value returned by the inverters
 * @param     byte_1 The first byte in the CAN message (LSB)
 * @param     byte_2 The second byte in the CAN message (MSB)
 */
int32_t INVLIB_parse_rpm(uint8_t byte_1, uint8_t byte_2);

#endif