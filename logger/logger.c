#include "logger.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"

#ifndef __weak
    #define __weak __attribute__((weak))
#endif


static LOG_LogLevelTypeDef _LOG_curr_loglevel = LOGLEVEL_INFO;
static bool _LOG_print_lineno = true;
static bool _LOG_print_prefix = true;
static bool _LOG_print_tick = false;


void _LOG_write(char*, uint32_t, LOG_LogLevelTypeDef, char*, ...);
void _LOG_write_raw(char*);


void LOG_init(LOG_LogLevelTypeDef loglevel, bool show_lineno, bool show_prefix, bool show_tick) {
    _LOG_curr_loglevel = loglevel;
    _LOG_print_lineno = show_lineno;
    _LOG_print_prefix = show_prefix;

    if (show_tick)
        LOG_write(LOGLEVEL_WARN, "LOG.show_tick not yet implemented");
}

void LOG_print_fenice_logo(char *subtitle) {
    _LOG_write_raw("\033[H");
    _LOG_write_raw("         E - A g l e   T r e n t o   R a c i n g   T e a m                ");
    _LOG_write_raw("██████████████████████████████     ████  ███  ████████████████████        ");
    _LOG_write_raw("                           ████   ███                                     ");
    _LOG_write_raw("  ████████████  ████████  ██████ ███   ███  ███         ████████    evo :)");
    _LOG_write_raw("      ███      ███       ███  █████   ███  ███        ███                 ");
    _LOG_write_raw("     ███      ████████  ███    ███   ███  █████████  █████████            ");
    _LOG_write_raw(subtitle);
    _LOG_write_raw("");
}

/**
 * @brief     Write a log message to UART in printf-style
 * 
 * @param     filename Internally macro-expanded to the caller's filename
 * @param     lineno Internally macro-expanded to the caller's line number
 * @param     loglevel Verbose level of the message for filtering
 * @param     fmt Format string
 * @param     ... Format string parameters
 */
void _LOG_write(char* filename, uint32_t lineno, LOG_LogLevelTypeDef loglevel, char *fmt, ...) {
    /* If not needed by current log level, return */
    if (loglevel < _LOG_curr_loglevel)
        return;

    /* Create buffer */
    uint16_t buf_size = 150;
    uint16_t offset = 0;
    char buffer[buf_size];

    if (_LOG_print_lineno) {
        /* Print source filename and line number */
        offset += snprintf(buffer, buf_size, "%s:%lu ", filename, lineno);
    }

    if (_LOG_print_prefix) {
        /* Print loglevel prefix */
        char* prefixes[] = {"[DBG] ", "[INF] ", "[WRN] ", "[ERR] "};
        offset += snprintf(
            buffer+offset, buf_size-offset,
            prefixes[loglevel]
        );
    }

    /* Print actual log message */
    va_list args;
    va_start(args, fmt);
    offset += vsnprintf(buffer+offset, buf_size-offset, fmt, args);
    va_end(args);

    /* Write message */
    _LOG_write_raw(buffer);

    if (offset >= buf_size)
        _LOG_write_raw("[WRN] [LOG] Previous log message was truncated due to insufficient buffer size");

}

__weak void _LOG_write_raw(char * txt) {
    /* This function must be redefined by the user */
    /* For example, to transmit buffer over UART */
}
