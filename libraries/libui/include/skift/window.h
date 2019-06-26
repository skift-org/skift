#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/painter.h>
#include <hideo/client.h>
#include <skift/events.h>

typedef struct
{
    hideo_window_flags_t flags;
    hideo_window_handle_t handle;

    int shared_memory;
    bitmap_t *backbuffer;
    painter_t *painter;

    rectangle_t bound;
} window_t;

window_t *window(void);

/* --- Methodes ------------------------------------------------------------- */

void window_show(window_t *this);

void window_event(event_t event);

void window_hide(window_t *this);

/* --- Getters and Setters -------------------------------------------------- */

int window_get_handle(window_t *this);

void window_set_title(window_t *this, const char *title);

void window_set_icon(window_t *this, const char *title);

point_t window_get_position(window_t *this);

void window_set_position(window_t *this, point_t pos);

point_t window_get_size(window_t *this);

void window_set_size(window_t *this, point_t pos);
