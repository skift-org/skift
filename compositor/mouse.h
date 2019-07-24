#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/widgets.h>
#include <skift/painter.h>

#include "renderer.h"
#include "assets.h"
#include "window.h"

typedef struct
{
    point_t position;
    mouse_cursor_state_t state;
} hideo_mouse_state_t;

typedef struct
{
    hideo_mouse_state_t paste;
    hideo_mouse_state_t current;

    bool has_mask;
    painter_t *painter;
    bitmap_t *mask;

    hideo_window_t* selected_window;
} hideo_mouse_t;

hideo_mouse_t *hideo_mouse(void);

void hideo_mouse_delete(hideo_mouse_t *this);

rectangle_t hideo_mouse_bound(hideo_mouse_t *this);

rectangle_t hideo_mouse_paste_bound(hideo_mouse_t *this);

void hideo_mouse_move(hideo_mouse_t *this, point_t offset, rectangle_t bound);

point_t hideo_mouse_get_position(hideo_mouse_t *this);

void hideo_mouse_set_position(hideo_mouse_t *this, point_t pos, rectangle_t bound);

void hideo_mouse_state(hideo_mouse_t *this, mouse_cursor_state_t state);

void hideo_mouse_invalidate_mask(hideo_mouse_t* this);

void hideo_mouse_repaint(hideo_mouse_t *this, hideo_renderer_t *render, hideo_assets_t *assets);

bool hideo_mouse_has_window_selected(hideo_mouse_t* this);

void hideo_mouse_begin_window_drag(hideo_mouse_t* this, hideo_window_t* win);

void hideo_mouse_end_window_drag(hideo_mouse_t* this);