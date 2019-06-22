#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/painter.h>

#include "compositor/renderer.h"
#include "compositor/assets.h"

#define HIDEO_WINDOW_TITLE_SIZE 64

typedef struct
{
    char title[64];
    rectangle_t bound;
    bool is_focused;
} hideo_window_t;

hideo_window_t *hideo_window(const char *title, rectangle_t bound);

void hideo_window_delete(hideo_window_t *this);

void hideo_window_move(hideo_window_t *this, point_t offset);

void hideo_window_set_size(hideo_window_t *this, point_t size);

void hideo_window_set_position(hideo_window_t *this, point_t pos);

rectangle_t hideo_window_get_client_area(hideo_window_t *this);

rectangle_t hideo_window_get_header_area(hideo_window_t *this);

bool hideo_window_get_focused(hideo_window_t *this);

void hideo_window_set_focused(hideo_window_t *this, bool focused);

void hideo_window_repaint(hideo_window_t *this, hideo_renderer_t *render, hideo_assets_t *assets);