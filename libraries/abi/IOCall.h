#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

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
    void *buffer;
    int buffer_width;
    int buffer_height;

    int blit_x;
    int blit_y;
    int blit_width;
    int blit_height;
} IOCallDisplayBlitArgs;

typedef enum
{
    IOCALL_TERMINAL_GET_SIZE,
    IOCALL_TERMINAL_SET_SIZE,

    IOCALL_DISPLAY_GET_MODE,
    IOCALL_DISPLAY_SET_MODE,
    IOCALL_DISPLAY_BLIT,

    __IOCALL_COUNT,
} IOCall;
