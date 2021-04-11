#include "demo/demos/Demos.h"

static RefPtr<Graphic::Bitmap> _test_image = nullptr;
static int _frame = 0;

void graphics_draw(Graphic::Painter &painter, Math::Recti screen, float time)
{
    UNUSED(time);

    if (_test_image == nullptr)
    {
        _test_image = Graphic::Bitmap::load_from_or_placeholder("/Applications/demo/test.png");
    }

    for (int x = 0; x < screen.width(); x++)
    {
        for (int y = 0; y < screen.height(); y++)
        {
            uint8_t pixel = x ^ y;

            painter.plot(screen.position() + Math::Vec2i(x, y), Graphic::Color::from_rgb_byte(pixel, pixel, pixel));
        }
    }

    Math::Recti test_image_bound(
        screen.x() + screen.width() / 2 - (_frame % screen.width()) / 2,
        screen.y() + screen.height() / 2 - (_frame % screen.height()) / 2,
        _frame % screen.width(),
        _frame % screen.height());

    painter.blit(*_test_image, _test_image->bound(), test_image_bound);

    painter.draw_rectangle(Math::Recti(75, 75, 100, 100), Graphic::Colors::WHITE);
    painter.fill_rectangle(Math::Recti(100, 100, 100, 100), Graphic::Colors::RED.with_alpha(0.5));
    painter.fill_rectangle(Math::Recti(125, 125, 100, 100), Graphic::Colors::GREEN.with_alpha(0.5));
    painter.fill_rectangle_rounded(Math::Recti(150, 150, 100, 100), 32, Graphic::Colors::BLUE.with_alpha(0.5));

    painter.draw_rectangle_rounded(screen, 64, 16, Graphic::Colors::RED.with_alpha(0.75));
    painter.draw_rectangle_rounded(screen, 64, 12, Graphic::Colors::YELLOW.with_alpha(0.75));
    painter.draw_rectangle_rounded(screen, 64, 8, Graphic::Colors::GREEN.with_alpha(0.75));
    painter.draw_rectangle_rounded(screen, 64, 4, Graphic::Colors::BLUE.with_alpha(0.75));

    painter.blur(Math::Recti(100, 100, 200, 200), 8);

    _frame++;
}
