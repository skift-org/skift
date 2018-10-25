#pragma once

#include <skift/types.h>
#include <skift/utils.h>

#define HEADER_HEIGHT 32
#define RESIZE_AREA    8
#define SNAP_AREA     32

typedef enum
{
    WINSTATE_FLOATING,
    WINSTATE_MINIMIZED,
    WINSTATE_MAXIMIZED,
    WINSTATE_TILED,
} hideo_winstate_t;

typedef enum 
{
    WINTYPE_MAIN,
    WINTYPE_POPOVER,
    WINTYPE_POPUP,
} hideo_wintype_t;

typedef PACKED(struct)
{
    int x;
    int y;
    
    uint width;
    uint height;

    uint floating_width;
    uint floating_height;

    hideo_winstate_t state;

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
        hideo_resize_t edge;
        bool horizontal;
        bool vertical;

        int offx;
        int offy;
    } resizestate;
} hideo_context_t;