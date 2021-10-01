# Logging helper library

Unite all logging useful functions in one small library

## Features:
- Filter messages by log level
	- `DEBUG`
	- `INFO`
	- `WARNING`
	- `ERROR`
- Show where a message was generated (file.c:line)
- Print Fenice's logo with an optional subtitle, eg.:
```
         E - A g l e   T r e n t o   R a c i n g   T e a m
██████████████████████████████     ███   ███  ████████████████████
                           ████   ███
  ████████████  ████████  ██████ ███   ███  ███         ████████
      ███      ███       ███  █████   ███  ███        ███
     ███      ████████  ███    ███   ███  █████████  █████████
            -    E C U   f i r m w a r e   v 1 . 0   -
```
- (In the future) centralize logging to serial and SD card

## Usage:
By deafault, options are set as follow:
- Log level: `INFO`
- Print source filename:line: `true`
- Print log level prefix: `true`
- Print time in ticks: `false` (not yet implemented)

If you want to customize these settings, just call:
```void LOG_init(LOG_LogLevelTypeDef loglevel, bool show_lineno, bool show_prefix, bool show_tick)```

To print messages, call:
```LOG_write(LOG_LogLevelTypeDef loglevel, char* fmt, ...)```
where the last two parameters allow to use it just like a printf function, eg.:
```LOG_write(LOGLEVEL_WARN, "[MODULE] Unexpected value on parameter %s: %d", param.name, param.val);```

