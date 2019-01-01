#pragma once

#include <skift/list.h>
#include <skift/drawing.h>

#define RESIZE_AREA    8
#define SNAP_AREA     32

#define WIN_TITLE_SIZE 32
#define WIN_HEADER_HEIGHT 32
#define WIN_RESIZE_AREA 8

typedef enum
{
    WINSTATE_FLOATING,
    WINSTATE_MINIMIZED,
    WINSTATE_MAXIMIZED,

    WINSTATE_TILED_LEFT,
    WINSTATE_TILED_RIGHT,
    WINSTATE_TILED_TOP,
    WINSTATE_TILED_BOTTOM,
} window_state_t;

typedef enum 
{
    WINTYPE_POPOVER,
    WINTYPE_POPUP,
} window_type_t;

typedef struct
{
    int x;
    int y;

    char title[WIN_TITLE_SIZE];

    uint width;
    uint height;

    window_state_t state;
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

typedef struct
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

    bool running;

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

hideo_window_t *hideo_window(hideo_context_t *ctx, const char * title, int x, int y, uint w, uint h);
void hideo_window_delete(hideo_context_t *ctx, hideo_window_t *win);

void hideo_window_draw(hideo_context_t *ctx, hideo_window_t *w);
void hideo_window_update(hideo_context_t *ctx, hideo_window_t *w, hideo_cursor_t *c);

int hideo_window_posx(hideo_context_t *ctx, hideo_window_t *win);
int hideo_window_posy(hideo_context_t *ctx, hideo_window_t *win);
uint hideo_window_width(hideo_context_t *ctx, hideo_window_t *win);
uint hideo_window_height(hideo_context_t *ctx, hideo_window_t *win);