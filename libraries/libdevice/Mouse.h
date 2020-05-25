#pragma once

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
