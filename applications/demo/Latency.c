
#include "demo/Demos.h"
#include <libsystem/System.h>

static int _last_tick = 0;
static int __i;

void latency_draw(Painter *painter, Rectangle screen, double time)
{
    __unused(time);

    int current_tick = system_get_ticks();

    painter_fill_rectangle(painter, screen, ALPHA(COLOR_BLACK, 0.005));
    painter_draw_line(painter, vec2i(screen.x, screen.y + 64), vec2i(screen.x + screen.width, screen.y + 64), ALPHA(COLOR_WHITE, 0.01));

    int x = __i % screen.width;
    __i++;

    for (int y = 0; y < abs(current_tick - _last_tick) * 4; y++)
    {
        painter_plot_pixel(painter, vec2i(screen.x + x, screen.y + y), COLOR_MAGENTA);
    }

    _last_tick = current_tick;
}
