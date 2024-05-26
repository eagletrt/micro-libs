/**
 * @file ucli.h
 * @brief Command Line Interface for embedded systems
 *
 * @date May 2024
 * @author Enrico Dalla Croce (Kalsifer-742) [kalsifer742@gmail.com]
 * @author Antonio Gelain [antonio.gelain@studenti.unitn.it]
 */

#ifndef UCLI_H
#define UCLI_H

// === Private Includes ===

#include <stdbool.h>
#include <stddef.h>

// === Parameters ===

#define UCLI_BUFFER_LEN 20
#define UCLI_COMMAND_N 10
#define UCLI_ARGS_N_MAX 10
#define UCLI_ARGS_LEN_MAX 10

// === Public types ===

typedef enum {
    UCLI_RETURN_CODE_OK,
    UCLI_RETURN_CODE_BUSY,
    UCLI_RETURN_CODE_MAX_COMMANDS_N_REACHED,
    UCLI_RETURN_CODE_ERROR,
} ucli_return_codes_t;

typedef struct {
    void (*send)(char* message, size_t size);
    void (*cs_enter)(void); // Can be left NULL
    void (*cs_exit)(void);  // Can be left NULL
    bool echo;
} ucli_handler_t;

typedef void (*ucli_command_function_t)(int argc, char args[][10]);

typedef struct {
    char name[UCLI_ARGS_N_MAX];
    ucli_command_function_t function;
} ucli_command_t;

// === Public functions ===

ucli_return_codes_t ucli_init(ucli_handler_t ucli_handler);
ucli_return_codes_t ucli_routine(void);
ucli_return_codes_t ucli_receive_data(char c);
ucli_return_codes_t ucli_add_command(ucli_command_t command);

#endif // UCLI_H
