#pragma once

#include <libgraphic/bitmap.h>
#include <libgraphic/painter.h>
#include <libsystem/io/Stream.h>

typedef struct
{
    Stream *device;
    Bitmap *backbuffer;
    Painter *painter;

    int width;
    int height;

    bool is_dirty;
    Rectangle dirty_bound;
} framebuffer_t;

framebuffer_t *framebuffer_open(void);

bool framebuffer_set_mode(framebuffer_t *this, int resx, int resy);

Rectangle framebuffer_bound(framebuffer_t *this);

void framebuffer_mark_dirty(framebuffer_t *this, Rectangle bound);

void framebuffer_mark_dirty_all(framebuffer_t *this);

void framebuffer_blit_dirty(framebuffer_t *this);

void framebuffer_blit(framebuffer_t *this);

void framebuffer_close(framebuffer_t *this);