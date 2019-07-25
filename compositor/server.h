#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/list.h>

#include <hjert/devices/mouse.h>
#include <hjert/devices/keyboard.h>

#include "mouse.h"
#include "stack.h"

typedef struct
{
    bool running;

    hideo_renderer_t *renderer;
    hideo_mouse_t *mouse;
    hideo_assets_t *assets;
    hideo_stack_t* stack;

    bitmap_t* wallpaper;

    list_t *clients;
} hideo_server_t;

hideo_server_t *hideo_server(void);

void hideo_server_delete(hideo_server_t *this);

/* --- Server painting ------------------------------------------------------ */

void hideo_server_repaint_all(hideo_server_t* this);

void hideo_server_blit_all(hideo_server_t* this);

bool hideo_server_running(hideo_server_t* this);

/* --- Windows -------------------------------------------------------------- */

void hideo_server_create_window(hideo_server_t *this, const char *title);

/* --- Mouse events handles ------------------------------------------------- */

void hideo_server_mouse_move(hideo_server_t *this, mouse_move_event_t *event);

void hideo_server_mouse_scroll(hideo_server_t *this, mouse_scroll_event_t *event);

void hideo_server_mouse_btnup(hideo_server_t *this, mouse_button_event_t *event);

void hideo_server_mouse_btndown(hideo_server_t *this, mouse_button_event_t *event);

/* --- Wallpaper ------------------------------------------------------------ */

void hideo_server_set_wallpaper(hideo_server_t* this, const char* path);