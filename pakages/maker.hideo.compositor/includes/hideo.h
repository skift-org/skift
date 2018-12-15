#pragma once

#include <skift/types.h>
#include <skift/utils.h>


#define RESIZE_AREA    8
#define SNAP_AREA     32

typedef enum
{
    BTN_PRESSED,
    BTN_RELEASED,
    BTN_UP,
    BTN_DOWN,
} hideo_button_state_t;

typedef enum 
{
    CURSOR_POINTER,
    CURSOR_DRAG,
    CURSOR_RESIZEH,
    CURSOR_RESIZEV,
    CURSOR_RESIZEHV,
} hideo_cursor_state_t;

typedef enum
{
    WIN_RESIZE_H,
    WIN_RESIZE_V,
    WIN_RESIZE_HV,
} hideo_resize_t;

typedef PACKED(struct)
{    
    int x;
    int y;
    
    hideo_cursor_state_t state;

    hideo_button_state_t leftbtn;
    hideo_button_state_t rightbtn;
    hideo_button_state_t middlebtn;
} hideo_cursor_t;

typedef struct 
{
    uint width;
    uint height;

    hideo_window_t *focus;
    list_t *windows;

    bitmap_t * screen;

    struct 
    {
        hideo_window_t * dragged;
        int offx;
        int offy;
    } dragstate;

    struct 
    {
        hideo_window_t * resized;
        hideo_resize_t edge;
        bool horizontal;
        bool vertical;

        int offx;
        int offy;
    } resizestate;
} hideo_context_t;