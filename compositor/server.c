/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "compositor/server.h"
#include "compositor/window.h"

#define server_painter this->renderer->painter

hideo_server_t *hideo_server(void)
{
    hideo_server_t *this = OBJECT(hideo_server);

    this->running = true;

    this->renderer = hideo_renderer((point_t){800, 600});
    this->assets = hideo_assets();
    this->mouse = hideo_mouse();
    this->stack = hideo_stack();

    this->wallpaper = NULL;

    this->clients = list();

    return this;
}

void hideo_server_delete(hideo_server_t *this)
{
    object_release(this->renderer);
    object_release(this->assets);
    object_release(this->mouse);
    object_release(this->stack);

    list_delete(this->clients, LIST_RELEASE_VALUES);
}

/* --- Server methodes ------------------------------------------------------ */

void hideo_server_repaint_all(hideo_server_t *this)
{
    if (this->wallpaper != NULL)
    {
        painter_blit_bitmap(
            server_painter,
            this->wallpaper,
            bitmap_bound(this->wallpaper),
            hideo_renderer_screen_bound(this->renderer));
    }
    else
    {
        painter_clear_rect(server_painter, hideo_renderer_screen_bound(this->renderer), COLOR_DIMGRAY);
    }

    hideo_stack_repaint(this->stack, this->renderer, this->assets);

    hideo_mouse_invalidate_mask(this->mouse);
    hideo_mouse_repaint(this->mouse, this->renderer, this->assets);
    hideo_renderer_dirty(this->renderer, hideo_renderer_screen_bound(this->renderer));
}

void hideo_server_blit_all(hideo_server_t *this)
{
    hideo_renderer_blit(this->renderer);
}

bool hideo_server_running(hideo_server_t *this)
{
    return this->running;
}

/* --- Windows -------------------------------------------------------------- */

static int window_create_index = 0;

void hideo_server_create_window(hideo_server_t *this, const char *title)
{
    hideo_window_t *win = hideo_window(title, (rectangle_t){{50 * (1 + window_create_index % 5), 50 * (1 + window_create_index % 5), 300, 100}});

    hideo_stack_add_window(this->stack, win);

    object_release(win);

    window_create_index++;

    hideo_server_repaint_all(this);
}

/* --- Mouse events handles ------------------------------------------------- */

void hideo_server_mouse_move(hideo_server_t *this, mouse_move_event_t *event)
{
    hideo_mouse_move(this->mouse, (point_t){event->offx, event->offy}, hideo_renderer_screen_bound(this->renderer));

    if (hideo_mouse_has_window_selected(this->mouse))
    {
        hideo_server_repaint_all(this);
        //hideo_window_repaint(this->mouse->selected_window, this->renderer, this->assets);
        hideo_mouse_invalidate_mask(this->mouse);
    }

    hideo_mouse_repaint(this->mouse, this->renderer, this->assets);
}

void hideo_server_mouse_scroll(hideo_server_t *this, mouse_scroll_event_t *event)
{
    UNUSED(this);
    UNUSED(event);
}

void hideo_server_mouse_btnup(hideo_server_t *this, mouse_button_event_t *event)
{
    UNUSED(this);
    UNUSED(event);

    if (event->button == MOUSE_BUTTON_LEFT)
    {
        hideo_mouse_end_window_drag(this->mouse);
        hideo_server_repaint_all(this);
    }
}

void hideo_server_mouse_btndown(hideo_server_t *this, mouse_button_event_t *event)
{
    UNUSED(this);
    UNUSED(event);

    if (event->button == MOUSE_BUTTON_LEFT)
    {
        hideo_window_t *window = hideo_stack_window_at(this->stack, hideo_mouse_get_position(this->mouse));

        if (window != NULL)
        {
            hideo_stack_window_set_focus(this->stack, window);
            hideo_mouse_begin_window_drag(this->mouse, window);
            hideo_server_repaint_all(this);
        }
    }
}

/* --- Wallpaper ------------------------------------------------------------ */

void hideo_server_set_wallpaper(hideo_server_t *this, const char *path)
{
    this->wallpaper = bitmap_load_from(path);
    hideo_mouse_invalidate_mask(this->mouse);
    hideo_server_repaint_all(this);
}