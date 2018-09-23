#pragma once

#include <types.h>
#include <utils.h>

typedef PACKED(struct)
{
    uint x;
    uint y;
    
    uint width;
    uint height;
} hideo_window_t;