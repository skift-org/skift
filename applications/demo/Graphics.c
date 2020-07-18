
#include <libsystem/Assert.h>

#include "demo/Demos.h"

static Bitmap *_test_image = NULL;
static int _frame = 0;

void graphics_draw(Painter *painter, Rectangle screen, double time)
{
    __unused(time);

    if (_test_image == NULL)
    {
        _test_image = bitmap_load_from("/System/skift-black.png");
    }

    for (int x = 0; x < screen.width; x++)
    {
        for (int y = 0; y < screen.height; y++)
        {
            painter_plot_pixel(painter, vec2i(screen.x + x, screen.y + y), (Color){{x ^ y, x ^ y, x ^ y, 255}});
        }
    }

    Rectangle test_image_bound = (Rectangle){
        {
            screen.x + screen.width / 2 - (_frame % screen.width) / 2,
            screen.y + screen.height / 2 - (_frame % screen.height) / 2,
            _frame % screen.width,
            _frame % screen.height,
        }};

    painter_blit_bitmap(painter, _test_image, bitmap_bound(_test_image), test_image_bound);

    painter_draw_rectangle(painter, (Rectangle){{75, 75, 100, 100}}, COLOR_WHITE);
    painter_fill_rectangle(painter, (Rectangle){{100, 100, 100, 100}}, ALPHA(COLOR_RED, 0.5));
    painter_fill_rectangle(painter, (Rectangle){{125, 125, 100, 100}}, ALPHA(COLOR_GREEN, 0.5));
    painter_fill_rounded_rectangle(painter, (Rectangle){{150, 150, 100, 100}}, 32, ALPHA(COLOR_BLUE, 0.5));

    painter_draw_rounded_rectangle(painter, screen, 64, 16, ALPHA(COLOR_RED, 0.75));
    painter_draw_rounded_rectangle(painter, screen, 64, 12, ALPHA(COLOR_YELLOW, 0.75));
    painter_draw_rounded_rectangle(painter, screen, 64, 8, ALPHA(COLOR_GREEN, 0.75));
    painter_draw_rounded_rectangle(painter, screen, 64, 4, ALPHA(COLOR_BLUE, 0.75));

    painter_blur_rectangle(painter, RECTANGLE(100, 100, 200, 200), 8);

    _frame++;
}
