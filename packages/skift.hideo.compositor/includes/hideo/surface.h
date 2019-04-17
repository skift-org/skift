#pragma once

#include <gfx/drawing.h>

typedef struct
{
    const char* title;

    uint height;
    uint width;

    bitmap_t* buffer;
} hideo_surface_t;