/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

// main.c - The Hideo compositor and window manager.

#include <stdlib.h>

#include <math.h>
#include <string.h>

#include <skift/io.h>
#include <skift/drawing.h>
#include <skift/list.h>
#include <skift/lock.h>

#include "hideo.h"

bool check_colision(int x0, int y0, int w0, int h0, int x1, int y1, int w1, int h1)
{
    return x0 < x1 + w1 && x1 < x0 + w0 && y0 < y1 + h1 && y1 < y0 + h0;
}

hideo_window_t *selected;

list_t *winstack;
lock_t winstacklock;

/* --- Windows -------------------------------------------------------------- */

hideo_window_t *hideo_create_window(char *title, int x, int y, int w, int h)
{
    hideo_window_t *win = MALLOC(hideo_window_t);

    win->title = title;

    win->x = x;
    win->y = y;
    win->width = w;
    win->height = h;

    list_pushback(winstack, (void *)win);
    selected = win;

    printf("Window@%x create", win);

    return win;
}

void hideo_window_update(hideo_context_t *ctx, hideo_window_t *w, hideo_cursor_t *c)
{
    UNUSED(ctx);

    if (check_colision(w->x, w->y, w->width, w->height, c->x, c->y, 1, 1) && c->leftbtn)
    {
        selected = w;
    }
}

void hideo_window_draw(hideo_context_t *ctx, hideo_window_t *w)
{
    drawing_fillrect(ctx->screen, w->x, w->y, w->width, w->height, 0xf5f5f5);
    drawing_fillrect(ctx->screen, w->x, w->y, w->width, 32, 0xffffff);

    if (w == selected)
    {
        drawing_rect(ctx->screen, w->x, w->y, w->width, w->height, 1, 0x0A64CD);
    }
    else
    {
        drawing_rect(ctx->screen, w->x, w->y, w->width, w->height, 1, 0x939393);
    }

    drawing_text(ctx->screen, w->title, w->x + (w->width / 2) - (strlen(w->title) * 8) / 2, w->y + 8, 0x0);
}

/* --- Mouse cursor --------------------------------------------------------- */

mouse_state_t mstate;

hideo_window_t *hideo_cursor_window(hideo_context_t *ctx, hideo_cursor_t *cur)
{
    hideo_window_t *result = NULL;

    FOREACH(w, winstack)
    {
        hideo_window_t *window = (hideo_window_t *)w->value;

        if (check_colision(window->x, window->y, window->width, window->height, cur->x, cur->y, 1, 1) && c->leftbtn)
        {
            result = window;
        }
    }

    return result;
}

void hideo_cursor_update(hideo_context_t *ctx, hideo_cursor_t *c)
{

    sk_io_mouse_get_state(&mstate);

    mstate.x = max(min(mstate.x, (int)ctx->width - 1), 0);
    mstate.y = max(min(mstate.y, (int)ctx->height - 1), 0);

    c->x = mstate.x;
    c->y = mstate.y;

    c->leftbtn = mstate.left;
    c->rightbtn = mstate.right;
    c->middlebtn = mstate.center;

    sk_io_mouse_set_state(&mstate);
}

void hideo_cursor_draw(hideo_context_t *ctx, hideo_cursor_t *c)
{
#define SCALE 2

    drawing_filltri(ctx->screen, c->x, c->y, c->x, c->y + 12 * SCALE, c->x + 8 * SCALE, c->y + 8 * SCALE, 0xffffff);

    drawing_line(ctx->screen, c->x, c->y, c->x, c->y + 12 * SCALE, 1, 0x0);
    drawing_line(ctx->screen, c->x, c->y, c->x + 8 * SCALE, c->y + 8 * SCALE, 1, 0x0);
    drawing_line(ctx->screen, c->x, c->y + 12 * SCALE, c->x + 8 * SCALE, c->y + 8 * SCALE, 1, 0x0);
}

/* --- Hideo ---------------------------------------------------------------- */

hideo_context_t *hideo_ctor(uint screen_width, uint screen_height)
{
    hideo_context_t *ctx = MALLOC(hideo_context_t);

    ctx->width = screen_width;
    ctx->height = screen_height;

    ctx->screen = bitmap_ctor(screen_width, screen_height);
    drawing_clear(ctx->screen, 0x0);

    return ctx;
}

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    sk_io_print("Hideo compositor and window manager");

    uint width, height = 0;
    sk_io_graphic_size(&width, &height);

    winstack = list_alloc();
    sk_lock_init(winstacklock);

    hideo_context_t *ctx = hideo_ctor(width, height);

    hideo_cursor_t cur = {.x = ctx->width / 2, .y = ctx->height / 2};

    hideo_create_window("Hello, world!", 54, 96, 256, 128);
    hideo_create_window("Good bye!", 300, 128, 256, 128);

    while (1)
    {
        // Update
        hideo_cursor_update(ctx, &cur);

        // Draw
        drawing_clear(ctx->screen, 0xe5e5e5);

        FOREACH(w, winstack)
        {
            hideo_window_t *window = (hideo_window_t *)w->value;
            hideo_window_update(ctx, window, &cur);
            hideo_window_draw(ctx, window);
        }

        hideo_cursor_draw(ctx, &cur);

        // Blit
        sk_io_graphic_blit(ctx->screen->buffer);
    }

    return 0;
}
