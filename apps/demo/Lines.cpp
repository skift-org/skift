
#include <libutils/Random.h>

#include "demo/Demos.h"

struct Line
{
    Vec2i start;
    Vec2i finish;
    Graphic::Color color;
};

static Random _random = {};

void lines_draw(Graphic::Painter &painter, Recti screen, float time)
{
    __unused(time);

    painter.fill_rectangle(screen, Graphic::Colors::BLACK.with_alpha(0.05));

    for (size_t i = 0; i < 16; i++)
    {
        Line line = {};

        line.start = Vec2i(_random.next_u32(screen.width()),
                           _random.next_u32(screen.height()));

        line.finish = Vec2i(_random.next_u32(screen.width()),
                            _random.next_u32(screen.height()));

        line.color = Graphic::Color::from_hsv(_random.next_u32(360), 0.75, 0.9);

        painter.draw_line(line.start, line.finish, line.color);
    }
}
