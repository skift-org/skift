/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include "demo/Demos.h"

void colors_draw(Painter *painter, Rectangle screen, double time)
{
    for (int x = 0; x < screen.width; x++)
    {
        for (int y = 0; y < screen.height; y++)
        {
            Color color = HSV((x / (float)screen.width) * 360.0 + time, (y / (float)screen.height), 1.0);
            painter_plot_pixel(painter, (Point){screen.x + x, screen.y + y}, color);
        }
    }
}
