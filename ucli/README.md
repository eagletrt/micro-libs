# μCLI

Command Line Interface for embedded systems.

Designed by

- Antonio Gelain [antonio.gelain@studenti.unitn.it]

Coded by

- Enrico Dalla Croce (Kalsifer-742) [kalsifer742@gmail.com]

## Documentation

WIP (if in need ask to Kalsifer-742)

## Usage

```C
#include "ucli.h"
#include <stdio.h>
#include <string.h>

uint8_t serial_rx_buffer = 0;
uint32_t primask = 0;

void cs_enter(void) {
    primask = __get_PRIMASK();
    __disable_irq();
}

void cs_exit(void) {
    if (!primask)
        __enable_irq();
}

void serial_tx(char* message, size_t size) {
    HAL_UART_Transmit(&huart2, (uint8_t*)message, size, SERIAL_TIMEOUT);
}

void print(int argc, char args[][10]) {
    if (argc > 1) {
        char* error_message = "\r\nprint expects 1 argument\r\n";
        serial_tx(error_message, strlen(error_message));
    } else {
        char* fmt = "\n\r%s\n\r";
        int size = snprintf(NULL, 0, fmt, args[0]);
        char message[size + 1]; // +1 for terminating null byte
        snprintf(message, sizeof message, fmt, args[0]);

        serial_tx(message, size);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
    ucli_receive_data((char)serial_rx_buffer);
    HAL_UART_Receive_IT(&huart2, &serial_rx_buffer, 1);
}

int main(void) {
    ucli_handler_t ucli_handler = {
        .send = &serial_tx,
        .cs_enter = NULL,
        .cs_exit = NULL,
        .echo = true,
    };

    ucli_init(ucli_handler);
    ucli_command_t echo_command = {.name = "print", .function = &print};
    ucli_add_command(echo_command);

    HAL_UART_Receive_IT(&huart2, &serial_rx_buffer, 1);

    while (1) {
        cs_enter();
        ucli_routine();
        cs_exit();
    }
}
```
