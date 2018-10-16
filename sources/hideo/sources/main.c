/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

// main.c - The Hideo compositor and window manager.

#include <stdlib.h>

#include <math.h>
#include <skift/io.h>
#include <skift/drawing.h>

#include "hideo.h"
#include "hideo-internal.h"

/* --- Windows -------------------------------------------------------------- */

void hideo_window_blit(hideo_context_t *ctx, hideo_window_t *w)
{
    drawing_fillrect(ctx->screen, w->x, w->y, w->width, w->height, 0xcfcfcf);
    drawing_rect(ctx->screen, w->x, w->y, w->width, w->height, 2, 0x939393);
    drawing_text(ctx->screen, "Hello, world!", w->x + 4, w->y + 4, 0x0);
}

/* --- Mouse cursor --------------------------------------------------------- */

void hideo_cursor_update(hideo_context_t *ctx, hideo_cursor_t *c)
{
    sk_io_mouse_get_position(&c->x, &c->y);

    c->x = max(min(c->x, (int)ctx->width - 1), 0);
    c->y = max(min(c->y, (int)ctx->height - 1), 0);
    
    sk_io_mouse_set_position(c->x, c->y);
}

void hideo_cursor_draw(hideo_context_t *ctx, hideo_cursor_t *c)
{
    drawing_filltri(ctx->screen, c->x, c->y, c->x, c->y + 12, c->x + 8, c->y + 8, 0xffffff);

    drawing_line(ctx->screen, c->x, c->y, c->x, c->y + 12, 1, 0x0);
    drawing_line(ctx->screen, c->x, c->y, c->x + 8, c->y + 8, 1, 0x0);
    drawing_line(ctx->screen, c->x, c->y + 12, c->x + 8, c->y + 8, 1, 0x0);
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

    hideo_context_t * ctx = hideo_ctor(width, height);

    hideo_window_t win = {.x = 64, .y = 64, .width = 256, .height = 256};
    hideo_cursor_t cur = {.x = ctx->width / 2, .y = ctx->height / 2};

    while (1)
    {
        // Update
        hideo_cursor_update(ctx, &cur);

        // Draw
        drawing_clear(ctx->screen, 0x0);

        hideo_window_blit(ctx, &win);
        hideo_cursor_draw(ctx, &cur);

        // Blit
        sk_io_graphic_blit(ctx->screen->buffer);
    }

    return 0;
}
