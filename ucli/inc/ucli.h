/**
 * @file ucli.h
 * @brief 
 *
 * @date 04 Apr 2024
 * @author Name Surname [your@email.here]
 */

#ifndef UCLI_H
#define UCLI_H

#include "ucli_fsm.h"
#include <stdint.h>

// Features
#define ENABLE_ECHO 0

// Parameters
#define BUFFER_LEN 10

void ucli_init(void);
void ucli_routine(uint8_t* byte);

#endif  // UCLI_H

