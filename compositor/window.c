/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/theme.h>
#include <skift/cstring.h>
#include <skift/logger.h>

#include "compositor/window.h"

static const spacing_t window_decoration_spacing = (spacing_t){32, 1, 1, 1};
static const point_t window_min_size = (point_t){128, 33};

hideo_window_t *hideo_window(const char *title, rectangle_t bound)
{
    hideo_window_t *this = OBJECT(hideo_window);

    strlcpy(this->title, title, HIDEO_WINDOW_TITLE_SIZE);
    this->bound = bound;
    this->is_focused = false;

    return this;
}

void hideo_window_delete(hideo_window_t *this)
{
    UNUSED(this);
}

void hideo_window_set_size(hideo_window_t *this, point_t size)
{
    this->bound = rectangle_min_size(
        (rectangle_t){
            .position = this->bound.position,
            .size = size,
        },
        window_min_size);
}

void hideo_window_set_position(hideo_window_t *this, point_t pos)
{
    this->bound.position = pos;
}

void hideo_window_move(hideo_window_t *this, point_t offset)
{
    this->bound.position = point_add(this->bound.position, offset);
}

rectangle_t hideo_window_get_client_area(hideo_window_t *this)
{
    return rectangle_shrink(this->bound, window_decoration_spacing);
}

rectangle_t hideo_window_get_header_area(hideo_window_t *this)
{
    return (rectangle_t){
        .position = this->bound.position,
        .size = (point_t){
            this->bound.width,
            window_decoration_spacing.top,
        }};
}

bool hideo_window_get_focused(hideo_window_t *this)
{
    return this->is_focused;
}

void hideo_window_set_focused(hideo_window_t *this, bool focused)
{
    logger_log(LOG_DEBUG, "window @%08x got %s: %d", this, focused ? "focused" : "unfocused");
    this->is_focused = focused;
}

void hideo_window_repaint(hideo_window_t *this, hideo_renderer_t *render, hideo_assets_t *assets)
{
    // Draw the hearder
    painter_fill_rect(render->painter, hideo_window_get_client_area(this), COLOR_BLACK);
    painter_fill_rect(render->painter, hideo_window_get_header_area(this), THEME_BACKGROUND);

    painter_draw_line(
        render->painter,
        (point_t){this->bound.X, this->bound.Y + window_decoration_spacing.top - 1},
        (point_t){this->bound.X + this->bound.width, this->bound.Y + window_decoration_spacing.top - 1},
        RGBA(1, 1, 1, 0.1));

    painter_draw_line(
        render->painter,
        (point_t){this->bound.X, this->bound.Y + window_decoration_spacing.top},
        (point_t){this->bound.X + this->bound.width, this->bound.Y + window_decoration_spacing.top},
        RGBA(0, 0, 0, 0.1));

    int text_width = font_measure_width(assets->font_sans, 16, this->title, strlen(this->title));
    painter_draw_text(
        render->painter,
        assets->font_sans,
        this->title,
        strlen(this->title),
        point_add(this->bound.position,
                  (point_t){this->bound.width / 2 - text_width / 2, 16 + 8}),
        16,
        THEME_FOREGROUND);

    // Draw the border
    if (this->is_focused)
    {
        painter_draw_rect(render->painter, this->bound, THEME_ACCENT);
    }
    else
    {
        painter_draw_rect(render->painter, this->bound, THEME_DISABLED);
    }

    hideo_renderer_dirty(render, this->bound);
}