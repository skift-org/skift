#include <libsystem/io/Path.h>
#include <libsystem/system/System.h>

#include "demo/Demos.h"

static int _last_tick = 0;
static int __i;

void latency_draw(Painter &painter, Rectangle screen, double time)
{
    __unused(time);

    int current_tick = system_get_ticks();

    painter.fill_rectangle(screen, ALPHA(COLOR_BLACK, 0.005));
    painter.draw_line(Vec2i(screen.x(), screen.y() + 64), Vec2i(screen.x() + screen.width(), screen.y() + 64), ALPHA(COLOR_WHITE, 0.01));

    int x = __i % screen.width();
    __i++;

    for (int y = 0; y < abs(current_tick - _last_tick) * 4; y++)
    {
        painter.plot_pixel(Vec2i(screen.x() + x, screen.y() + y), COLOR_MAGENTA);
    }

    _last_tick = current_tick;
}
