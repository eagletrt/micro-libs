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
#define UCLI_ARGS_N_MAX 50
#define UCLI_ARGS_LEN_MAX 10
#define UCLI_PROMPT ("> ")

// clang-format off
//             ________    ____
//      __  __/ ____/ /   /  _/
//     / / / / /   / /    / /
//    / /_/ / /___/ /____/ /
//   / ____/\____/_____/___/
//  / /
// /_/

#define WELCOME_MESSAGE     \
"\
\r\n\
            ________    ____\r\n\
     __  __/ ____/ /   /  _/\r\n\
    / / / / /   / /    / /\r\n\
   / /_/ / /___/ /____/ /\r\n\
  / ____/\\____/_____/___/\r\n\
 / /\r\n\
/_/\r\n\
\n\
# The new generation CLI!\r\n\
\n\
"
// clang-format on

// === Public types ===

typedef enum {
    UCLI_RETURN_CODE_OK,
    UCLI_RETURN_CODE_BUSY,
    UCLI_RETURN_CODE_FUN_PTR_IS_NULL,
    UCLI_RETURN_CODE_MAX_COMMANDS_N_REACHED,
    UCLI_RETURN_CODE_ERROR,
} ucli_return_codes_t;

typedef struct {
    void (*enable_receive)(void);
    void (*send)(char* message, size_t size);
    void (*cs_enter)(void); // Can be left NULL
    void (*cs_exit)(void);  // Can be left NULL
    bool echo;
} ucli_handler_t;

typedef void (*ucli_command_function_t)(int argc, char args[][50]);

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
