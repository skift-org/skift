#pragma once

#include "types.h"

typedef enum
{
    MOUSE_LEFT,
    MOUSE_RIGH,
    MOUSE_CENTER
} mouse_button_t;

typedef enum
{
    MOUSE_PRESSED,
    MOUSE_RELEASED,
    MOUSE_HELD,
    MOUSE_UP
} mouse_bstate_t;

void mouse_setup();

void mouse_get_position(uint * outxpos, uint * outypos);
int mouse_set_position(uint xpos, uint ypos);
mouse_bstate_t mouse_button(mouse_button_t button);