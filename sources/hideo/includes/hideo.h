#pragma once

#include <skift/types.h>
#include <skift/utils.h>

typedef PACKED(struct)
{
    uint x;
    uint y;
    
    uint width;
    uint height;
} hideo_window_t;