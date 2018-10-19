#pragma once

typedef PACKED(struct) 
{
    int x;
    int y;
    int scroll;

    bool left;
    bool right;
    bool middle;
} mouse_state_t;