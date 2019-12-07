#pragma once

#include <libgraphic/bitmap.h>
#include <libgraphic/painter.h>
#include <libsystem/iostream.h>

typedef struct
{
    IOStream *device;
    Bitmap *backbuffer;
    Painter *painter;

    int width;
    int height;

    bool is_dirty;
    rectangle_t dirty_bound;
} framebuffer_t;

framebuffer_t *framebuffer_open(void);

bool framebuffer_set_mode(framebuffer_t *this, int resx, int resy);

rectangle_t framebuffer_bound(framebuffer_t *this);

void framebuffer_mark_dirty(framebuffer_t *this, rectangle_t bound);

void framebuffer_mark_dirty_all(framebuffer_t *this);

void framebuffer_blit_dirty(framebuffer_t *this);

void framebuffer_blit(framebuffer_t *this);

void framebuffer_close(framebuffer_t *this);