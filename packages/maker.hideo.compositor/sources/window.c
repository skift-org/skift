
#include <math.h>
#include <string.h>
#include <skift/logger.h>
#include <skift/list.h>
#include <skift/drawing.h>
#include <skift/color.h>

#include "hideo.h"

hideo_window_t *hideo_window(hideo_context_t *ctx, const char *title, int x, int y, uint w, uint h)
{
    hideo_window_t *win = MALLOC(hideo_window_t);

    strncpy(win->title, title, WIN_TITLE_SIZE);

    win->x = x;
    win->y = y;
    win->width = w;
    win->height = h;

    win->state = WINSTATE_FLOATING;

    list_pushback(ctx->windows, (void *)win);
    ctx->focus = win;

    sk_log(LOG_DEBUG, "Window { @%x, TITLE='%s'} create", win, win->title);

    return win;
}

void hideo_window_delete(hideo_context_t *ctx, hideo_window_t *win)
{
    sk_log(LOG_DEBUG, "Window { @%x, TITLE='%s'} deleted", win, win->title);

    list_remove(ctx->windows, (void *)win);
    free(win);
}

/* --- Update and Draw ------------------------------------------------------ */

void hideo_window_update(hideo_context_t *ctx, hideo_window_t *w, hideo_cursor_t *c)
{
    if (ctx->dragstate.dragged == w)
    {
        ctx->dragstate.dragged->x = c->x + ctx->dragstate.offx;
        ctx->dragstate.dragged->y = c->y + ctx->dragstate.offy;
    }

    if (ctx->resizestate.resized == w)
    {
        if (ctx->resizestate.horizontal)
            w->width = max(c->x - w->x + ctx->resizestate.offx, 256);

        if (ctx->resizestate.vertical)
            w->height = max(c->y - w->y + ctx->resizestate.offy, WIN_HEADER_HEIGHT);
    }
}

void hideo_window_draw(hideo_context_t *ctx, hideo_window_t *win)
{
    int winx = hideo_window_posx(ctx, win);
    int winy = hideo_window_posy(ctx, win);
    uint winwidth = hideo_window_width(ctx, win);
    uint winheight = hideo_window_height(ctx, win);


    if (win == ctx->focus)
    {
        drawing_fillrect(ctx->screen, winx, winy + WIN_HEADER_HEIGHT, winwidth, winheight - WIN_HEADER_HEIGHT, COLOR_WHITESMOKE);
        drawing_fillrect(ctx->screen, winx, winy, winwidth, WIN_HEADER_HEIGHT, COLOR_WHITE);
        drawing_rect(ctx->screen, winx, winy, winwidth, winheight, COLOR_BLUE);
        drawing_text(ctx->screen, win->title, winx + (winwidth / 2) - (strlen(win->title) * 8) / 2, winy + 8, COLOR_BLACK);
    }
    else
    {
        drawing_fillrect(ctx->screen, winx, winy, winwidth, winheight, COLOR_WHITESMOKE);
        drawing_rect(ctx->screen, winx, winy, winwidth, winheight, COLOR_GREY);
        drawing_text(ctx->screen, win->title, winx + (winwidth / 2) - (strlen(win->title) * 8) / 2, winy + 8, COLOR_GREY);
    }
}

/* --- Window geometry ------------------------------------------------------ */

int hideo_window_posx(hideo_context_t *ctx, hideo_window_t *win)
{
    switch (win->state)
    {
    case WINSTATE_FLOATING:
        return win->x;

    case WINSTATE_TILED_RIGHT:
        return ctx->width / 2;

    default:
        return 0;
    }
}

int hideo_window_posy(hideo_context_t *ctx, hideo_window_t *win)
{
    switch (win->state)
    {
    case WINSTATE_FLOATING:
        return win->y;

    case WINSTATE_TILED_BOTTOM:
        return ctx->height / 2;

    default:
        return 0;
    }
}

uint hideo_window_width(hideo_context_t *ctx, hideo_window_t *win)
{
    switch (win->state)
    {
    case WINSTATE_MINIMIZED:
    case WINSTATE_FLOATING:
        return win->width;

    case WINSTATE_TILED_LEFT:
    case WINSTATE_TILED_RIGHT:
        return ctx->width / 2;

    case WINSTATE_MAXIMIZED:
    case WINSTATE_TILED_TOP:
    case WINSTATE_TILED_BOTTOM:
        return ctx->width;

    default:
        return 256;
    }
}

uint hideo_window_height(hideo_context_t *ctx, hideo_window_t *win)
{
    switch (win->state)
    {
    case WINSTATE_MINIMIZED:
    case WINSTATE_FLOATING:
        return win->height;

    case WINSTATE_TILED_TOP:
    case WINSTATE_TILED_BOTTOM:
        return ctx->height / 2;

    case WINSTATE_MAXIMIZED:
    case WINSTATE_TILED_LEFT:
    case WINSTATE_TILED_RIGHT:
        return ctx->height;

    default:
        return 256;
    }
}