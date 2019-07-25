#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/bitmap.h>
#include <skift/painter.h>
#include <skift/iostream.h>

#define HIDEO_RENDERER_MAX_DIRTY 32

typedef struct
{
    iostream_t *device;

    bitmap_t *backbuffer;
    painter_t *painter;

    int dirty_index;
    rectangle_t dirty[HIDEO_RENDERER_MAX_DIRTY];
} hideo_renderer_t;

hideo_renderer_t *hideo_renderer(point_t prefered_resolution);

void hideo_renderer_delete(hideo_renderer_t *this);

rectangle_t hideo_renderer_screen_bound(hideo_renderer_t* this);

void hideo_renderer_dirty(hideo_renderer_t *this, rectangle_t rect);

void hideo_renderer_blit(hideo_renderer_t *this);
