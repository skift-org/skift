/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/io/Stream.h>

#include "demo/Demos.h"

static Stream *random_device = NULL;

typedef struct
{
    Point start;
    Point finish;
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

    line.start.X = screen.X + abs((int)line.start.X % screen.width);
    line.start.Y = screen.Y + abs((int)line.start.Y % screen.height);
    line.finish.X = screen.X + abs((int)line.finish.X % screen.width);
    line.finish.Y = screen.Y + abs((int)line.finish.Y % screen.height);

    line.color.A = 255;

    painter_draw_line(painter, line.start, line.finish, line.color);
}
