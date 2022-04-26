#ifndef CTRL_NWK_UTILS_H
#define CTRL_NWK_UTILS_H

#include "stdint.h"
#include "stdbool.h"

#define CTRL_DLE 0x10           /*< Data Link Escape ctrl char */
#define CTRL_STX 0x02           /*< Start of Text ctrl char */
#define CTRL_ETX 0x03           /*< End of Text ctrl char */

#define CTRL_PARAMID_DREQ       0x00
#define CTRL_PARAMID_STEER_ANG  0x01
#define CTRL_PARAMID_CSW        0x02
#define CTRL_PARAMID_YAW        0x03
#define CTRL_PARAMID_O_RR       0x04
#define CTRL_PARAMID_O_RL       0x05
#define CTRL_PARAMID_O_FR       0x06
#define CTRL_PARAMID_O_FL       0x07
#define CTRL_PARAMID_AX_G       0x08
#define CTRL_PARAMID_BRAKE      0x09
#define CTRL_PARAMID_TMRR       0x0A
#define CTRL_PARAMID_TMLL       0x0B

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
