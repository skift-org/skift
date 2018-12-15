#pragma once

#include <skift/generic.h>

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

typedef struct
{
    int x;
    int y;

    char title[WIN_TITLE_SIZE];

    uint width;
    uint height;

    window_state_t state;
} hideo_window_t;

#include "hideo.h"

hideo_window_t *hideo_window(hideo_context_t *ctx, const char * title, int x, int y, uint w, uint h);
void hideo_window_delete(hideo_context_t *ctx, hideo_window_t *win);

void hideo_window_draw(hideo_context_t *ctx, hideo_window_t *w);
void hideo_window_update(hideo_context_t *ctx, hideo_window_t *w, hideo_cursor_t *c);

int hideo_window_posx(hideo_context_t *ctx, hideo_window_t *win);
int hideo_window_posy(hideo_context_t *ctx, hideo_window_t *win);
uint hideo_window_width(hideo_context_t *ctx, hideo_window_t *win);
uint hideo_window_height(hideo_context_t *ctx, hideo_window_t *win);