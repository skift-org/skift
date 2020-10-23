#include <libsystem/system/System.h>
#include <libutils/Path.h>

#include "demo/Demos.h"

static int _last_tick = 0;
static int __i;

void latency_draw(Painter &painter, Rectangle screen, float time)
{
    __unused(time);

    int current_tick = system_get_ticks();

    painter.fill_rectangle(screen, Colors::BLACK.with_alpha(0.005));
    painter.draw_line(Vec2i(screen.x(), screen.y() + 64), Vec2i(screen.x() + screen.width(), screen.y() + 64), Colors::WHITE.with_alpha(0.01));

    int x = __i % screen.width();
    __i++;

    for (int y = 0; y < abs(current_tick - _last_tick) * 4; y++)
    {
        painter.plot_pixel(Vec2i(screen.x() + x, screen.y() + y), Colors::MAGENTA);
    }

    _last_tick = current_tick;
}
