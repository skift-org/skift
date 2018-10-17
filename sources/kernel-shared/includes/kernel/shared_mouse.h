#pragma once

typedef PACKED(struct) 
{
    int x;
    int y;
    int scroll;

    bool left;
    bool right;
    bool center;
} mouse_state_t;