#pragma once

#include <libsystem/Common.h>

typedef struct
{
    int width;
    int height;
} IOCallTerminalSizeArgs;

typedef struct
{
    int width;
    int height;
} IOCallDisplayModeArgs;

typedef struct
{
    uint32_t *buffer;
    int buffer_width;
    int buffer_height;

    int blit_x;
    int blit_y;
    int blit_width;
    int blit_height;
} IOCallDisplayBlitArgs;

typedef struct
{
    void *keymap;
    size_t size;
} IOCallKeyboardSetKeymapArgs;

typedef struct
{
    int width;
    int height;
    int cursor_x;
    int cursor_y;
} IOCallTextModeStateArgs;

typedef enum
{
    IOCALL_TERMINAL_GET_SIZE,
    IOCALL_TERMINAL_SET_SIZE,

    IOCALL_DISPLAY_GET_MODE,
    IOCALL_DISPLAY_SET_MODE,
    IOCALL_DISPLAY_BLIT,

    IOCALL_KEYBOARD_SET_KEYMAP,
    IOCALL_KEYBOARD_GET_KEYMAP,

    IOCALL_TEXTMODE_GET_STATE,
    IOCALL_TEXTMODE_SET_STATE,

    __IOCALL_COUNT,
} IOCall;
