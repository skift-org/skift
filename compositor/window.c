/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/theme.h>

#include "compositor/window.h"
#include "compositor/assets.h"

spacing_t window_decoration_spacing = (spacing_t){32, 1, 1, 1};

rectangle_t window_get_client_area(rectangle_t bound)
{
    return rectangle_shrink(bound, window_decoration_spacing);
}

rectangle_t window_get_header_area(rectangle_t bound)
{
    return (rectangle_t){
        .position = bound.position,
        .size = (point_t){
            bound.width,
            window_decoration_spacing.top,
        }};
}

void window_paint_decoration(painter_t *paint, rectangle_t bound, const char *title)
{
    // Draw the hearder
    // painter_fill_rect(paint, window_get_header_area(bound), THEME_BACKGROUND);
    painter_fill_rect(paint, bound, THEME_BACKGROUND);

    painter_draw_line(paint, (point_t){bound.X, bound.Y + window_decoration_spacing.top - 1}, (point_t){bound.X + bound.width, bound.Y + window_decoration_spacing.top - 1}, RGBA(1, 1, 1, 0.1));
    painter_draw_line(paint, (point_t){bound.X, bound.Y + window_decoration_spacing.top}, (point_t){bound.X + bound.width, bound.Y + window_decoration_spacing.top}, RGBA(0, 0, 0, 0.1));

    int text_width = font_measure_width(hideo_assets_get_font(), 14, title, strlen(title));
    painter_draw_text(
        paint,
        hideo_assets_get_font(),
        title,
        strlen(title),
        point_add(bound.position,
                  (point_t){bound.width / 2 - text_width / 2, 14 + 8}),
        14,
        THEME_FOREGROUND);

    // Draw the border
    painter_draw_rect(paint, bound, THEME_ACCENT);
}