#pragma once

enum MouseButtonState
{
    MOUSE_BUTTON_RELEASED,
    MOUSE_BUTTON_PRESSED,
};

struct MousePacket
{
    int offx;
    int offy;
    int scroll;

    MouseButtonState left;
    MouseButtonState right;
    MouseButtonState middle;
};
