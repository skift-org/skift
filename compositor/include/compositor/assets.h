#pragma once;

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/bitmap.h>
#include <skift/widgets.h>
#include <skift/font.h>

void hideo_assets_load(void);

bitmap_t *hideo_assets_get_mouse(mouse_cursor_state_t state);

font_t *hideo_assets_get_font(void);
