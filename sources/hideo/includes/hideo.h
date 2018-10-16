#pragma once

#include <skift/types.h>
#include <skift/utils.h>

typedef struct 
{
    uint width;
    uint height;
    bitmap_t * screen;
} hideo_context_t;

typedef PACKED(struct)
{
    int x;
    int y;
    
    uint width;
    uint height;
} hideo_window_t;

typedef PACKED(struct)
{
    int x;
    int y;
} hideo_cursor_t;

