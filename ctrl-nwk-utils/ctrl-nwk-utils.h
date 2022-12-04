#ifndef CTRL_NWK_UTILS_H
#define CTRL_NWK_UTILS_H

#include "stdint.h"
#include "stdbool.h"


#define CTRL_PAYLOAD_SIZE 7     /*< Payload size: header (1) + body (4) + CRC (2) */
#define CTRL_MAX_FRAME_LEN 18   /*< Maximum frame size if fully byte-stuffed: 2 + 14 + 2 */

#define CTRL_DLE 0x10           /*< Data Link Escape ctrl char */
#define CTRL_STX 0x02           /*< Start of Text ctrl char */
#define CTRL_ETX 0x03           /*< End of Text ctrl char */

#define CTRL_PARAMID_DREQ       ((uint8_t)0x00)
#define CTRL_PARAMID_STEER_ANG  ((uint8_t)0x01)
#define CTRL_PARAMID_YAW        ((uint8_t)0x02)
#define CTRL_PARAMID_O_RR       ((uint8_t)0x03)
#define CTRL_PARAMID_O_RL       ((uint8_t)0x04)
#define CTRL_PARAMID_O_FR       ((uint8_t)0x05)
#define CTRL_PARAMID_O_FL       ((uint8_t)0x06)
#define CTRL_PARAMID_AX_G       ((uint8_t)0x07)
#define CTRL_PARAMID_BRAKE      ((uint8_t)0x08)
#define CTRL_PARAMID_PW_MAP     ((uint8_t)0x09)
#define CTRL_PARAMID_SC_MAP     ((uint8_t)0x0A)
#define CTRL_PARAMID_TV_MAP     ((uint8_t)0x0B)
#define CTRL_PARAMID_UBAR       ((uint8_t)0x0C)
#define CTRL_PARAMID_TRMAX      ((uint8_t)0x0D)
#define CTRL_PARAMID_TLMAX      ((uint8_t)0x0E)
#define CTRL_PARAMID_TRIGHT     ((uint8_t)0x0F)
#define CTRL_PARAMID_TLEFT      ((uint8_t)0x10)


typedef enum {
    CTRL_Mode_None,
    CTRL_Mode_Slip,
    CTRL_Mode_Torque,
    CTRL_Mode_Complete
} CTRL_ModeTypeDef;

typedef struct {
    uint8_t ParamID;
    float ParamVal;
    uint16_t CRC16;
} CTRL_PayloadTypeDef;


bool CTRL_is_frame_wellformed(uint8_t*, uint8_t);

bool CTRL_read_frame(uint8_t*, uint8_t, CTRL_PayloadTypeDef*);

uint8_t CTRL_unstuff_buffer(uint8_t*, uint8_t, uint8_t*);

uint8_t CTRL_stuff_buffer(uint8_t*, uint8_t, uint8_t*);

uint8_t CTRL_compose_frame(CTRL_PayloadTypeDef*, uint8_t*);

#endif
