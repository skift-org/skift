#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

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
