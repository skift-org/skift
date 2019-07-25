/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "mouse.h"

const point_t mouse_bound = {56, 56};

hideo_mouse_t *hideo_mouse(void)
{
    hideo_mouse_t *this = OBJECT(hideo_mouse);

    this->mask = bitmap(mouse_bound.X, mouse_bound.Y);
    this->painter = painter(this->mask);
    this->has_mask = false;

    this->current = (hideo_mouse_state_t){
        .position = point_zero,
        .state = MOUSE_CURSOR_STATE_DEFAULT,
    };

    this->paste = this->current;

    this->selected_window = NULL;

    return this;
}

void hideo_mouse_delete(hideo_mouse_t *this)
{
    object_release(this->selected_window);
    object_release(this->painter);
    object_release(this->mask);
}

rectangle_t hideo_mouse_bound(hideo_mouse_t *this)
{
    return (rectangle_t){
        .position = point_sub(this->current.position, point_div(mouse_bound, 2)),
        .size = mouse_bound,
    };
}

rectangle_t hideo_mouse_paste_bound(hideo_mouse_t *this)
{
    return (rectangle_t){
        .position = point_sub(this->paste.position, point_div(mouse_bound, 2)),
        .size = mouse_bound,
    };
}

static point_t mouse_offsets[] = {
    [MOUSE_CURSOR_STATE_DEFAULT] = point_zero,
    [MOUSE_CURSOR_STATE_DISABLED] = point_zero,
    [MOUSE_CURSOR_STATE_BUSY] = {-14, -14},
    [MOUSE_CURSOR_STATE_TEXT] = {-14, 0},
    [MOUSE_CURSOR_STATE_MOVE] = {-14, -14},
    [MOUSE_CURSOR_STATE_RESIZEH] = {-14, -14},
    [MOUSE_CURSOR_STATE_RESIZEV] = {-14, -14},
    [MOUSE_CURSOR_STATE_RESIZEHV] = {-14, -14},
    [MOUSE_CURSOR_STATE_RESIZEVH] = {-14, -14},
};

void hideo_mouse_move(hideo_mouse_t *this, point_t offset, rectangle_t bound)
{
    this->current.position = point_add(this->current.position, offset);
    this->current.position = point_clamp_to_rect(this->current.position, bound);

    if (this->selected_window != NULL)
    {
        hideo_window_move(this->selected_window, offset);
    }
}

point_t hideo_mouse_get_position(hideo_mouse_t *this)
{
    return this->current.position;
}

void hideo_mouse_set_position(hideo_mouse_t *this, point_t pos, rectangle_t bound)
{
    this->current.position = point_clamp_to_rect(pos, bound);
}

void hideo_mouse_state(hideo_mouse_t *this, mouse_cursor_state_t state)
{
    this->current.state = state;
}

void hideo_mouse_invalidate_mask(hideo_mouse_t* this)
{
    this->has_mask = false;
}

void hideo_mouse_repaint(hideo_mouse_t *this, hideo_renderer_t *render, hideo_assets_t *assets)
{
    // Cleanup the backbuffer
    if (this->has_mask)
    {
        painter_blit_bitmap(render->painter, this->mask, bitmap_bound(this->mask), hideo_mouse_paste_bound(this));
        hideo_renderer_dirty(render, hideo_mouse_paste_bound(this));
    }

    // Save the backbuffer
    painter_blit_bitmap(this->painter, render->backbuffer, hideo_mouse_bound(this), bitmap_bound(this->mask));
    this->has_mask = true;

    // Draw the mouse cursor to the backbuffer
    point_t offset = mouse_offsets[this->current.state];

    bitmap_t *cursor_asset = assets->mouse_cursors[this->current.state];

    rectangle_t dest = (rectangle_t){
        .position = point_add(this->current.position, offset),
        .size = bitmap_bound(cursor_asset).size,
    };

    painter_blit_bitmap(render->painter, cursor_asset, bitmap_bound(cursor_asset), dest);
    hideo_renderer_dirty(render, dest);

    this->paste = this->current;
}

bool hideo_mouse_has_window_selected(hideo_mouse_t* this)
{
    return this->selected_window != NULL;
}

void hideo_mouse_begin_window_drag(hideo_mouse_t* this, hideo_window_t* win)
{
    this->selected_window = object_retain(win);
}

void hideo_mouse_end_window_drag(hideo_mouse_t* this)
{
    if (this->selected_window != NULL)
    {
        object_release(this->selected_window);
        this->selected_window = NULL;
    }
}