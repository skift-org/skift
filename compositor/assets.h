#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/bitmap.h>
#include <skift/widgets.h>
#include <skift/font.h>

typedef struct
{
    font_t* font_sans;
    bitmap_t *mouse_cursors[MOUSE_CURSOR_STATE_COUNT];
} hideo_assets_t;

hideo_assets_t* hideo_assets(void);

void hideo_assets_delete(hideo_assets_t* this);
