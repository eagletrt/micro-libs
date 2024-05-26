# μCLI

Command Line Interface for embedded systems.

Designed by

- Antonio Gelain [antonio.gelain@studenti.unitn.it]

Coded by

- Enrico Dalla Croce (Kalsifer-742) [kalsifer742@gmail.com]

## Documentation

> [!NOTE]
> The documentation is non existent. If you need help contact **Kalsifer-742**

> [!TIP]
> If you get `UCLI_RETURN_CODE_MAX_COMMANDS_N_REACHED` error and you need to add more commands go to the file `ucli-dictionary.h` and change the parameters.
>
> ```C
> // === Parameters ===
> 
> #define BUCKETS_N 15
> #define BUCKETS_SIZE 5
> ```
>
> I tested this parameters and they are a good default up to `30+` commands.

> [!IMPORTANT]
> If you find a bug please open an issue and assign it to me.

## Usage

> [!TIP]
> This example uses Interrupts which is the recommended way to handle the serial communication

---

### Code

```C
// Library Include

#include "ucli.h"

// Critical section handlers for the internal RingBuffer of the CLI

uint32_t primask = 0;

void cs_enter(void) {
    primask = __get_PRIMASK();
    __disable_irq();
}

void cs_exit(void) {
    if (!primask)
        __enable_irq();
}

// Hardware dependent functions for sending and receiving data trough serial

void serial_rx(void) { HAL_UART_Receive_IT(&huart2, &serial_rx_buffer, 1); }

#define SERIAL_TIMEOUT 100
void serial_tx(char* message, size_t size) {
    while (!(huart2.gState == HAL_UART_STATE_READY))
        ;
    HAL_UART_Transmit(&huart2, (uint8_t*)message, size, SERIAL_TIMEOUT);
}

int8_t serial_rx_buffer = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
    if (ucli_receive_data((char)serial_rx_buffer) != UCLI_RETURN_CODE_OK) {
        return;
    }
}

// Example of a CLI command

void print(int argc, char args[][10]) {
    if (argc == 0 || argc > 1) {
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

// main Example

int main(void) {
    ucli_handler_t ucli_handler = {
        .enable_receive = &serial_rx,
        .send = &serial_tx,
        .cs_enter = &cs_enter,
        .cs_exit = &cs_exit,
        .echo = true,
    };

    if (ucli_init(ucli_handler) != UCLI_RETURN_CODE_OK) {
        return 1;
    }
    ucli_command_t print_command = {.name = "print", .function = &print};
    if (ucli_add_command(print_command) != UCLI_RETURN_CODE_OK) {
        // The commands structure is full
        return 1;
    }

    serial_rx();

    while (1) {
        ucli_routine();
    }
}
```
