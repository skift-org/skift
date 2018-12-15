/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

// main.c - The Hideo compositor and window manager.

#include <stdlib.h>

#include <math.h>
#include <string.h>

#include <skift/drawing.h>
#include <skift/logger.h>
#include <skift/io.h>
#include <skift/list.h>
#include <skift/lock.h>

#include "hideo_window.h"
#include "hideo.h"

bool check_colision(int x0, int y0, int w0, int h0, int x1, int y1, int w1, int h1)
{
    return x0 < x1 + w1 && x1 < x0 + w0 && y0 < y1 + h1 && y1 < y0 + h0;
}

hideo_window_t *hideo_window_at(hideo_context_t *ctx, int x, int y, bool header)
{
    hideo_window_t *result = NULL;

    FOREACH(w, ctx->windows)
    {
        hideo_window_t *win = (hideo_window_t *)w->value;

        int winx = hideo_window_posx(ctx, win);
        int winy = hideo_window_posy(ctx, win);
        uint winwidth = hideo_window_width(ctx, win);
        uint winheight = hideo_window_height(ctx, win);

        if (check_colision(winx, winy, winwidth, header ? WIN_HEADER_HEIGHT : winheight, x, y, 1, 1))
        {
            result = win;
        }
    }

    return result;
}

void hideo_cursor_update(hideo_context_t *ctx, hideo_cursor_t *c)
{
    static mouse_state_t old_state;
    static mouse_state_t new_state;

    old_state = new_state;
    sk_io_mouse_get_state(&new_state);

    new_state.x = max(min(new_state.x, (int)ctx->width - 1), 0);
    new_state.y = max(min(new_state.y, (int)ctx->height - 1), 0);

    sk_io_mouse_set_state(&new_state);
    c->x = new_state.x;
    c->y = new_state.y;

    c->rightbtn = new_state.right ? BTN_DOWN : BTN_UP;
    c->middlebtn = new_state.middle ? BTN_DOWN : BTN_UP;

    // Left button
    if (new_state.left && !old_state.left)
    {
        c->leftbtn = BTN_PRESSED;
    }
    else if (!new_state.left && old_state.left)
    {
        c->leftbtn = BTN_RELEASED;
    }
    else
    {
        c->leftbtn = new_state.left ? BTN_DOWN : BTN_UP;
    }

    // Right button
    if (new_state.right && !old_state.right)
    {
        c->rightbtn = BTN_PRESSED;
    }
    else if (!new_state.right && old_state.right)
    {
        c->rightbtn = BTN_RELEASED;
    }
    else
    {
        c->rightbtn = new_state.right ? BTN_DOWN : BTN_UP;
    }

    // middle button.
    if (new_state.middle && !old_state.middle)
    {
        c->middlebtn = BTN_PRESSED;
    }
    else if (!new_state.middle && old_state.middle)
    {
        c->middlebtn = BTN_RELEASED;
    }
    else
    {
        c->middlebtn = new_state.middle ? BTN_DOWN : BTN_UP;
    }

    hideo_window_t *win = hideo_window_at(ctx, c->x, c->y, false);

    if (win != NULL)
    {
        if (c->leftbtn == BTN_PRESSED)
        {
            if (ctx->focus != win)
            {
                sk_log(LOG_DEBUG, "Window@%x got the focus.", win);
                ctx->focus = win;
            }

            list_remove(ctx->windows, win);
            list_pushback(ctx->windows, win);

            int winx = hideo_window_posx(ctx, win);
            int winy = hideo_window_posy(ctx, win);
            uint winwidth = hideo_window_width(ctx, win);
            // uint winheight = hideo_window_height(ctx, win);

            if (win->state == WINSTATE_FLOATING && check_colision(winx + win->width - RESIZE_AREA, win->y + win->height - RESIZE_AREA, RESIZE_AREA, RESIZE_AREA, c->x, c->y, 1, 1))
            {
                c->state = CURSOR_RESIZEHV;
                ctx->resizestate.resized = win;

                ctx->resizestate.horizontal = true;
                ctx->resizestate.vertical = true;

                ctx->resizestate.offx = win->x + win->width - c->x;
                ctx->resizestate.offy = win->y + win->height - c->y;
            }
            else if (win->state == WINSTATE_FLOATING && check_colision(win->x, win->y + win->height - RESIZE_AREA, win->width, RESIZE_AREA, c->x, c->y, 1, 1))
            {
                c->state = CURSOR_RESIZEV;
                ctx->resizestate.resized = win;

                ctx->resizestate.horizontal = false;
                ctx->resizestate.vertical = true;

                ctx->resizestate.offx = win->x + win->width - c->x;
                ctx->resizestate.offy = win->y + win->height - c->y;
            }
            else if (win->state == WINSTATE_FLOATING && check_colision(win->x + win->width - RESIZE_AREA, win->y, RESIZE_AREA, win->height, c->x, c->y, 1, 1))
            {
                c->state = CURSOR_RESIZEH;
                ctx->resizestate.resized = win;

                ctx->resizestate.horizontal = true;
                ctx->resizestate.vertical = false;

                ctx->resizestate.offx = win->x + win->width - c->x;
                ctx->resizestate.offy = win->y + win->height - c->y;
            }
            else if (check_colision(winx, winy, winwidth, WIN_HEADER_HEIGHT, c->x, c->y, 1, 1))
            {
                ctx->dragstate.dragged = win;
                if (win->state == WINSTATE_FLOATING)
                {
                    ctx->dragstate.offx = winx - c->x;
                    ctx->dragstate.offy = winy - c->y;
                }
                else
                {
                    ctx->dragstate.offx = -(win->width / 2);
                    ctx->dragstate.offy = winy - c->y;
                }

                c->state = CURSOR_DRAG;
                win->state = WINSTATE_FLOATING;
            }
        }
    }

    if (c->leftbtn == BTN_RELEASED)
    {
        if (ctx->resizestate.resized != NULL)
        {
            c->state = CURSOR_POINTER;
            ctx->resizestate.resized = NULL;
        }

        if (ctx->dragstate.dragged != NULL)
        {
            hideo_window_t *dragged = ctx->dragstate.dragged;

            if (c->x < SNAP_AREA)
            {
                dragged->state = WINSTATE_TILED_LEFT;
            }
            else if (c->x > (int)(ctx->width - SNAP_AREA))
            {
                dragged->state = WINSTATE_TILED_RIGHT;
            }
            else if (c->y < SNAP_AREA)
            {
                dragged->state = WINSTATE_TILED_TOP;
            }
            else if (c->y > (int)(ctx->height - SNAP_AREA))
            {
                dragged->state = WINSTATE_TILED_BOTTOM;
            }
            else
            {
                dragged->state = WINSTATE_FLOATING;
            }

            c->state = CURSOR_POINTER;
            ctx->dragstate.dragged = NULL;
        }
    }
}

void hideo_cursor_draw(hideo_context_t *ctx, hideo_cursor_t *c)
{
#define SCALE 2

    switch (c->state)
    {
    case CURSOR_POINTER:
        drawing_filltri(ctx->screen, c->x, c->y, c->x, c->y + 12 * SCALE, c->x + 8 * SCALE, c->y + 8 * SCALE, 0xffffff);

        drawing_line(ctx->screen, c->x, c->y, c->x, c->y + 12 * SCALE, 0x0);
        drawing_line(ctx->screen, c->x, c->y, c->x + 8 * SCALE, c->y + 8 * SCALE, 0x0);
        drawing_line(ctx->screen, c->x, c->y + 12 * SCALE, c->x + 8 * SCALE, c->y + 8 * SCALE, 0x0);
        break;

    case CURSOR_DRAG:
        drawing_line(ctx->screen, c->x, c->y - 4 * SCALE, c->x, c->y + 4 * SCALE, 0x0);
        drawing_line(ctx->screen, c->x - 4 * SCALE, c->y, c->x + 4 * SCALE, c->y, 0x0);
        break;

    case CURSOR_RESIZEH:
        drawing_line(ctx->screen, c->x - 4 * SCALE, c->y, c->x + 4 * SCALE, c->y, 0x0);
        break;

    case CURSOR_RESIZEV:
        drawing_line(ctx->screen, c->x, c->y - 4 * SCALE, c->x, c->y + 4 * SCALE, 0x0);
        break;

    case CURSOR_RESIZEHV:
        drawing_line(ctx->screen, c->x - 4 * SCALE, c->y - 4 * SCALE, c->x + 4 * SCALE, c->y + 4 * SCALE, 0x0);
        break;

    default:
        break;
    }

    if (ctx->dragstate.dragged != NULL)
    {
        if (c->x < SNAP_AREA)
        {
            drawing_rect(ctx->screen, 0, 0, ctx->width / 2, ctx->height, 0x0A64CD);
        }
        else if (c->x > (int)(ctx->width - SNAP_AREA))
        {
            drawing_rect(ctx->screen, ctx->width / 2, 0, ctx->width / 2, ctx->height, 0x0A64CD);
        }
        else if (c->y < SNAP_AREA)
        {
            drawing_rect(ctx->screen, 0, 0, ctx->width, ctx->height / 2, 0x0A64CD);
        }
        else if (c->y > (int)(ctx->height - SNAP_AREA))
        {
            drawing_rect(ctx->screen, 0, ctx->height / 2, ctx->width, ctx->height / 2, 0x0A64CD);
        }
    }
}

/* --- Hideo ---------------------------------------------------------------- */

hideo_context_t *hideo_ctor(uint screen_width, uint screen_height)
{
    hideo_context_t *ctx = MALLOC(hideo_context_t);

    ctx->width = screen_width;
    ctx->height = screen_height;

    ctx->screen = bitmap(screen_width, screen_height);

    ctx->windows = list();
    ctx->focus = NULL;

    return ctx;
}

int program()
{
    uint width, height = 0;
    sk_io_graphic_size(&width, &height);

    if (width != 0 && height != 0)
    {
        sk_log(LOG_INFO, "Graphic context created %dx%d", width, height);

        hideo_context_t *ctx = hideo_ctor(width, height);

        hideo_cursor_t cur = {.x = ctx->width / 2, .y = ctx->height / 2};

        hideo_window(ctx, "Hello, world!", 54, 96, 256, 128);
        hideo_window(ctx, "Good bye!", 300, 128, 256, 128);
        hideo_window(ctx, "Wow such window!", 100, 200, 256, 256);

        while (1)
        {
            // Update
            hideo_cursor_update(ctx, &cur);

            FOREACH(w, ctx->windows)
            {
                hideo_window_t *window = (hideo_window_t *)w->value;
                hideo_window_update(ctx, window, &cur);
            }

            // Draw
            //drawing_clear(ctx->screen, 0xe5e5e5);
            drawing_text(ctx->screen, "maker.hideo.compositor", 8, 8, 0x939393);

            FOREACH(w, ctx->windows)
            {
                hideo_window_t *window = (hideo_window_t *)w->value;

                hideo_window_draw(ctx, window);
            }

            hideo_cursor_draw(ctx, &cur);

            // Blit
            sk_io_graphic_blit(ctx->screen->buffer);
        }

        return 0;
    }
    else
    {
        sk_log(LOG_FATAL, "No graphic context found, aborting");
        return -1;
    }
}
