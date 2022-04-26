#include "ctrl-nwk-utils.h"
#include "string.h" // For memcpy


/* Constants and private variables ------------------------------------------ */
#define CTRL_MAX_FRAME_LEN 18   /*< Maximum frame size if fully byte-stuffed */
#define CTRL_PAYLOAD_SIZE 7     /*< Payload size: header (1) + body (4) + CRC (2) */


/* Prototypes --------------------------------------------------------------- */
uint8_t _CTRL_is_frame_wellformed(uint8_t*, uint8_t);

/**
 * @brief     Read a raw frame buffer and put in `data` its parsed contents
 * 
 * @param     buf Raw frame bytes including preamble and stop bytes
 * @param     buf_size Number of bytes in the input buffer
 * @param     data Output struct
 * @return    bool True if no error occurred, false otherwise
 */
bool CTRL_read_frame(uint8_t *buf, uint8_t buf_size, CTRL_PayloadTypeDef *data) {
    uint8_t payload[CTRL_PAYLOAD_SIZE];
    CTRL_unstuff_buffer(
        buf + 2, // DLE + STX
        buf_size - 2, // DLE + ETX
        payload
    );

    uint8_t frame_size = _CTRL_is_frame_wellformed(buf, CTRL_MAX_FRAME_LEN);
    
    memcpy(&(data->ParamID), payload+0, 1);
    memcpy(&(data->ParamVal), payload+1, 4);
    memcpy(&(data->CRC), payload+5, 2);
    
    return frame_size > 0;
}

/**
 * @brief     Check if a frame is well formed and return its length (or 0)
 * @note      The frame needs to have already been unstuffed
 */
uint8_t _CTRL_is_frame_wellformed(uint8_t* buf, uint8_t max_len) {
    if (max_len < 2 || buf[0] != CTRL_DLE || buf[1] != CTRL_STX)
        return 0; // Error

    for (uint8_t idx = 2; ; idx++) {
        if (idx >= max_len)
            return 0;
        if (buf[idx-1] == CTRL_DLE && buf[idx] == CTRL_ETX)
            return idx;
    }
}

/**
 * @brief     Remove the byte-stuffed DLE symbols from a buffer
 * 
 * @param     in_buf Input buffer
 * @param     n Max bytes to scan in the input buffer
 * @param     out_buf Output buffer to which the result will be written
 * @return    uint8_t Number of written bytes
 */
uint8_t CTRL_unstuff_buffer(uint8_t *in_buf, uint8_t n, uint8_t *out_buf) {
    uint8_t i = 0, o = 0;
    
    while (i < n) {
        if (in_buf[i] == CTRL_DLE)
            i++;
        out_buf[o++] = in_buf[i++];
    }
    
    return o;
}

/**
 * @brief     Add DLE symbols to a buffer to escape existing ones
 * 
 * @param     in_buf Input buffer
 * @param     n Max bytes to scan in the input buffer
 * @param     out_buf Output buffer to which the result will be written
 * @return    uint8_t Number of written bytes
 */
uint8_t CTRL_stuff_buffer(uint8_t *in_buf, uint8_t n, uint8_t *out_buf) {
    uint8_t i = 0, o = 0;
    
    while (i < n) {
        if (in_buf[i] == CTRL_DLE)
            out_buf[o++] = CTRL_DLE;
        out_buf[o++] = in_buf[i++];
    }
    
    return o;
}

/**
 * @brief     Create a raw frame to be sent containing `data` serialized
 * 
 * @param     data Struct with payload data
 * @param     buf Output buffer
 * @return    uint8_t Raw frame size
 */
uint8_t CTRL_compose_frame(CTRL_PayloadTypeDef *data, uint8_t *buf) {
    uint8_t payload[7];
    memcpy(payload+0, &(data->ParamID), 1);
    memcpy(payload+1, &(data->ParamVal), 4);
    memcpy(payload+5, &(data->CRC), 2);

    uint8_t idx = 0;
    buf[idx++] = CTRL_DLE;
    buf[idx++] = CTRL_STX;
    idx += CTRL_stuff_buffer(payload, 7, buf+idx);
    buf[idx++] = CTRL_DLE;
    buf[idx++] = CTRL_ETX;
 
    return idx;
}
