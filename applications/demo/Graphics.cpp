
#include <libsystem/Assert.h>

#include "demo/Demos.h"

static Bitmap *_test_image = nullptr;
static int _frame = 0;

void graphics_draw(Painter &painter, Rectangle screen, double time)
{
    __unused(time);

    if (_test_image == nullptr)
    {
        _test_image = bitmap_load_from("/System/skift.png");
    }

    for (int x = 0; x < screen.width(); x++)
    {
        for (int y = 0; y < screen.height(); y++)
        {
            uint8_t pixel = x ^ y;

            painter.plot_pixel(screen.position() + Vec2i(x, y), (Color){{pixel, pixel, pixel, 255}});
        }
    }

    Rectangle test_image_bound(
        screen.x() + screen.width() / 2 - (_frame % screen.width()) / 2,
        screen.y() + screen.height() / 2 - (_frame % screen.height()) / 2,
        _frame % screen.width(),
        _frame % screen.height());

    painter.blit_bitmap(_test_image, bitmap_bound(_test_image), test_image_bound);

    painter.draw_rectangle(Rectangle(75, 75, 100, 100), COLOR_WHITE);
    painter.fill_rectangle(Rectangle(100, 100, 100, 100), ALPHA(COLOR_RED, 0.5));
    painter.fill_rectangle(Rectangle(125, 125, 100, 100), ALPHA(COLOR_GREEN, 0.5));
    painter.fill_rounded_rectangle(Rectangle(150, 150, 100, 100), 32, ALPHA(COLOR_BLUE, 0.5));

    painter.draw_rounded_rectangle(screen, 64, 16, ALPHA(COLOR_RED, 0.75));
    painter.draw_rounded_rectangle(screen, 64, 12, ALPHA(COLOR_YELLOW, 0.75));
    painter.draw_rounded_rectangle(screen, 64, 8, ALPHA(COLOR_GREEN, 0.75));
    painter.draw_rounded_rectangle(screen, 64, 4, ALPHA(COLOR_BLUE, 0.75));

    painter.blur_rectangle(Rectangle(100, 100, 200, 200), 8);

    _frame++;
}
