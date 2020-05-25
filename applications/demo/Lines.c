
#include <libsystem/io/Stream.h>

#include "demo/Demos.h"

static Stream *random_device = NULL;

typedef struct
{
    Vec2i start;
    Vec2i finish;
    Color color;
} Line;

void lines_draw(Painter *painter, Rectangle screen, double time)
{
    __unused(time);

    if (random_device == NULL)
    {
        random_device = stream_open("/dev/random", OPEN_READ);
    }

    Line line = {};
    stream_read(random_device, &line, sizeof(Line));

    line.start.x = screen.x + abs((int)line.start.x % screen.width);
    line.start.y = screen.y + abs((int)line.start.y % screen.height);
    line.finish.x = screen.x + abs((int)line.finish.x % screen.width);
    line.finish.y = screen.y + abs((int)line.finish.y % screen.height);

    line.color.A = 255;

    painter_fill_rectangle(painter, screen, ALPHA(COLOR_BLACK, 0.01));
    painter_draw_line_antialias(painter, line.start, line.finish, line.color);
}
