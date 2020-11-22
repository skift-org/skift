
#include <libsystem/Assert.h>

#include "demo/Demos.h"

static RefPtr<Bitmap> _test_image = nullptr;
static int _frame = 0;

void graphics_draw(Painter &painter, Rectangle screen, float time)
{
    __unused(time);

    if (_test_image == nullptr)
    {
        _test_image = Bitmap::load_from_or_placeholder("/Applications/demo/test.png");
    }

    for (int x = 0; x < screen.width(); x++)
    {
        for (int y = 0; y < screen.height(); y++)
        {
            uint8_t pixel = x ^ y;

            painter.plot_pixel(screen.position() + Vec2i(x, y), Color::from_byte(pixel, pixel, pixel));
        }
    }

    Rectangle test_image_bound(
        screen.x() + screen.width() / 2 - (_frame % screen.width()) / 2,
        screen.y() + screen.height() / 2 - (_frame % screen.height()) / 2,
        _frame % screen.width(),
        _frame % screen.height());

    painter.blit_bitmap(*_test_image, _test_image->bound(), test_image_bound);

    painter.draw_rectangle(Rectangle(75, 75, 100, 100), Colors::WHITE);
    painter.fill_rectangle(Rectangle(100, 100, 100, 100), Colors::RED.with_alpha(0.5));
    painter.fill_rectangle(Rectangle(125, 125, 100, 100), Colors::GREEN.with_alpha(0.5));
    painter.fill_rounded_rectangle(Rectangle(150, 150, 100, 100), 32, Colors::BLUE.with_alpha(0.5));

    painter.draw_rounded_rectangle(screen, 64, 16, Colors::RED.with_alpha(0.75));
    painter.draw_rounded_rectangle(screen, 64, 12, Colors::YELLOW.with_alpha(0.75));
    painter.draw_rounded_rectangle(screen, 64, 8, Colors::GREEN.with_alpha(0.75));
    painter.draw_rounded_rectangle(screen, 64, 4, Colors::BLUE.with_alpha(0.75));

    painter.blur_rectangle(Rectangle(100, 100, 200, 200), 8);

    _frame++;
}
