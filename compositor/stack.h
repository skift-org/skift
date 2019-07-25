#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/list.h>

#include "window.h"

typedef struct
{
    hideo_window_t *focused;
    list_t *windows;
} hideo_stack_t;

hideo_stack_t *hideo_stack(void);

void hideo_stack_delete(hideo_stack_t *this);

void hideo_stack_add_window(hideo_stack_t *this, hideo_window_t *win);

void hideo_stack_remove_window(hideo_stack_t *this, hideo_window_t *win);

hideo_window_t *hideo_stack_window_at(hideo_stack_t *this, point_t at);

hideo_window_t *hideo_stack_window_get_focus(hideo_stack_t *this);

void hideo_stack_window_set_focus(hideo_stack_t *this, hideo_window_t *win);

void hideo_stack_repaint(hideo_stack_t *this, hideo_renderer_t *render, hideo_assets_t *assets);
