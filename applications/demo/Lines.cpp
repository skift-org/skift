
#include <libsystem/system/Random.h>

#include "demo/Demos.h"

struct Line
{
    Vec2i start;
    Vec2i finish;
    Color color;
};

static Random random = {};
static bool random_initialized = false;

void lines_draw(Painter &painter, Rectangle screen, float time)
{
    __unused(time);

    if (!random_initialized)
    {
        random = random_create();
        random_initialized = true;
    }

    painter.fill_rectangle(screen, Colors::BLACK.with_alpha(0.05));

    for (size_t i = 0; i < 16; i++)
    {
        Line line = {};

        line.start = Vec2i(random_uint32_max(&random, screen.width()),
                           random_uint32_max(&random, screen.height()));

        line.finish = Vec2i(random_uint32_max(&random, screen.width()),
                            random_uint32_max(&random, screen.height()));

        line.color = Color::from_hex(random_uint32_max(&random, 0xffffff));

        painter.draw_line_antialias(line.start, line.finish, line.color);
    }
}
