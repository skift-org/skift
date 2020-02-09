#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#define MOUSE_DEVICE "/dev/mouse"

typedef enum
{
    MOUSE_BUTTON_RELEASED,
    MOUSE_BUTTON_PRESSED,
} MouseButtonState;

typedef struct
{
    int offx;
    int offy;
    int scroll;

    MouseButtonState left;
    MouseButtonState right;
    MouseButtonState middle;
} MousePacket;