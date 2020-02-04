#pragma once

typedef struct
{
    int width;
    int height;
} IOCallTerminalSizeArgs;

typedef enum
{
    IOCALL_TERMINAL_GET_SIZE,
    IOCALL_TERMINAL_SET_SIZE,

    __IOCALL_COUNT,
} IOCall;
