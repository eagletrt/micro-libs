#include "ctrl-nwk-utils.h"
#include "string.h" // For memcpy


/**
 * @brief     Read a raw frame buffer and put in `data` its parsed contents
 * 
 * @param     buf Raw frame bytes including preamble and stop bytes
 * @param     buf_size Number of bytes in the input buffer
 * @param     data Output struct
 * @return    bool True if no error occurred, false otherwise
 */
bool CTRL_read_frame(uint8_t *buf, uint8_t buf_size, CTRL_PayloadTypeDef *data) {
    /* Check that there are at least enough bytes: (dle+stx) + min_payload_size + (dle+etx) */
    if (buf_size < CTRL_PAYLOAD_SIZE + 4)
        return false;
        
    /* Check if the unstuffed buffer will fit */
    if (buf_size > CTRL_MAX_FRAME_LEN)
        return false;

    /* Check that the buffer actually represents a frame */
    if (!CTRL_is_frame_wellformed(buf, buf_size))
        return false;

    /* Unstuff the payload by trimming away dle+stx and dle+etx */
    uint8_t payload[CTRL_PAYLOAD_SIZE];
    uint8_t payload_size = CTRL_unstuff_buffer(
        buf + 2,
        buf_size - 4,
        payload
    );

    /* Check that a legal payload was extracted */
    if (payload_size != CTRL_PAYLOAD_SIZE)
        return false;
    
    memcpy(&(data->ParamID), payload+0, 1);
    memcpy(&(data->ParamVal), payload+1, 4);
    memcpy(&(data->CRC), payload+5, 2);
    
    return true;
}

/**
 * @brief     Check if a frame is well formed and return its length (or 0)
 * 
 * @param     buf Raw buffer from which to read the frame
 * @param     len Number of bytes in the buffer
 */
bool CTRL_is_frame_wellformed(uint8_t* buf, uint8_t len) {
    if (len < 2 || buf[0] != CTRL_DLE || buf[1] != CTRL_STX)
        return false;

    /* Look for the terminating sequence: a DLE+ETX with no preceding DLE */
    for (uint8_t idx = 3; idx < len; idx++) {
        if (buf[idx-1] == CTRL_DLE && buf[idx] == CTRL_ETX)
            if (buf[idx-2] != CTRL_DLE)
                return true;
    }
    
    return false;
}

/**
 * @brief     Remove the byte-stuffed DLE symbols from a buffer
 * 
 * @param     in_buf Input buffer
 * @param     n Number of bytes to scan in the input buffer
 * @param     out_buf Output buffer to which the result will be written
 * @return    uint8_t Number of written bytes
 */
uint8_t CTRL_unstuff_buffer(uint8_t *in_buf, uint8_t n, uint8_t *out_buf) {
    uint8_t i = 0, o = 0;
    
    while (i < n) {
        if (in_buf[i] == CTRL_DLE)
            i++;
        if (i < n)
            out_buf[o++] = in_buf[i++];
    }
    
    return o;
}

/**
 * @brief     Add DLE symbols to a buffer to escape existing ones
 * 
 * @param     in_buf Input buffer
 * @param     n Number of bytes to scan in the input buffer
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
