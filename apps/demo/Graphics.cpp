
#include "demo/Demos.h"

static RefPtr<Bitmap> _test_image = nullptr;
static int _frame = 0;

void graphics_draw(Painter &painter, Recti screen, float time)
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

            painter.plot(screen.position() + Vec2i(x, y), Color::from_byte(pixel, pixel, pixel));
        }
    }

    Recti test_image_bound(
        screen.x() + screen.width() / 2 - (_frame % screen.width()) / 2,
        screen.y() + screen.height() / 2 - (_frame % screen.height()) / 2,
        _frame % screen.width(),
        _frame % screen.height());

    painter.blit(*_test_image, _test_image->bound(), test_image_bound);

    painter.draw_rectangle(Recti(75, 75, 100, 100), Colors::WHITE);
    painter.fill_rectangle(Recti(100, 100, 100, 100), Colors::RED.with_alpha(0.5));
    painter.fill_rectangle(Recti(125, 125, 100, 100), Colors::GREEN.with_alpha(0.5));
    painter.fill_rectangle_rounded(Recti(150, 150, 100, 100), 32, Colors::BLUE.with_alpha(0.5));

    painter.draw_rectangle_rounded(screen, 64, 16, Colors::RED.with_alpha(0.75));
    painter.draw_rectangle_rounded(screen, 64, 12, Colors::YELLOW.with_alpha(0.75));
    painter.draw_rectangle_rounded(screen, 64, 8, Colors::GREEN.with_alpha(0.75));
    painter.draw_rectangle_rounded(screen, 64, 4, Colors::BLUE.with_alpha(0.75));

    painter.blur(Recti(100, 100, 200, 200), 8);

    _frame++;
}
