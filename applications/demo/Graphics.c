/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/assert.h>

#include "demo/Demos.h"

static Bitmap *_test_image = NULL;
static int _frame = 0;

void graphics_draw(Painter *painter, Rectangle screen, double time)
{
    __unused(time);

    if (_test_image == NULL)
    {
        _test_image = bitmap_load_from("/res/skift-black.png");
    }

    for (int x = 0; x < screen.width; x++)
    {
        for (int y = 0; y < screen.height; y++)
        {
            painter_plot_pixel(painter, (Point){screen.X + x, screen.Y + y}, (Color){{x ^ y, x ^ y, x ^ y, 255}});
        }
    }

    Rectangle test_image_bound = (Rectangle){
        {
            screen.X + screen.width / 2 - (_frame % screen.width) / 2,
            screen.Y + screen.height / 2 - (_frame % screen.height) / 2,
            _frame % screen.width,
            _frame % screen.height,
        }};

    painter_blit_bitmap(painter, _test_image, bitmap_bound(_test_image), test_image_bound);

    painter_draw_rectangle(painter, (Rectangle){{75, 75, 100, 100}}, COLOR_WHITE);
    painter_fill_rectangle(painter, (Rectangle){{100, 100, 100, 100}}, ALPHA(COLOR_RED, 0.5));
    painter_fill_rectangle(painter, (Rectangle){{125, 125, 100, 100}}, ALPHA(COLOR_GREEN, 0.5));
    painter_fill_rectangle(painter, (Rectangle){{150, 150, 100, 100}}, ALPHA(COLOR_BLUE, 0.5));

    _frame++;
}
