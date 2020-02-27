#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/Bitmap.h>
#include <libgraphic/Painter.h>
#include <libsystem/io/Handle.h>

typedef struct
{
    Handle handle;

    Bitmap *backbuffer;
    Painter *painter;

    int width;
    int height;

    bool is_dirty;
    Rectangle dirty_bound;
} Framebuffer;

Framebuffer *framebuffer_open(void);

Result framebuffer_set_mode(Framebuffer *framebuffer, int width, int height);

Rectangle framebuffer_bound(Framebuffer *framebuffer);

void framebuffer_mark_dirty(Framebuffer *framebuffer, Rectangle bound);

void framebuffer_mark_dirty_all(Framebuffer *framebuffer);

void framebuffer_blit_dirty(Framebuffer *framebuffer);

void framebuffer_blit(Framebuffer *framebuffer);

void framebuffer_close(Framebuffer *framebuffer);