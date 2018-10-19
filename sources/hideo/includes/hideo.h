#pragma once

#include <skift/types.h>
#include <skift/utils.h>

#define HEADER_HEIGHT 32

typedef PACKED(struct)
{
    int x;
    int y;
    
    uint width;
    uint height;

    char * title;
} hideo_window_t;

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
    CURSOR_RESIZE_HORIZONTAL,
    CURSOR_RESIZE_VERTICAL,
} hideo_cursor_state_t;

typedef enum
{
    WIN_EDGE_TOP,
    WIN_EDGE_BOTTOM,
    WIN_EDGE_LEFT,
    WIN_EDGE_RIGHT,
} hideo_window_edge_t;

typedef PACKED(struct)
{    
    int x;
    int y;
    
    hideo_cursor_state_t state;

    hideo_button_state_t leftbtn;
    hideo_button_state_t rightbtn;
    hideo_button_state_t middlebtn;
} hideo_cursor_t;

typedef PACKED(struct)
{

} hideo_keyboard_t;

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
        hideo_window_edge_t edge;
        bool horizontal;
        bool vertical;
    } resizestate;
} hideo_context_t;