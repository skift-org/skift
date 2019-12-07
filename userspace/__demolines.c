/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/iostream.h>
#include <libsystem/error.h>

#include <libgraphic/framebuffer.h>

typedef struct
{
    Point start;
    Point finish;
    Color color;
} line_t;

Rectangle line_bound(line_t line)
{
    Rectangle bound;

    bound.X = min(line.start.X, line.finish.X);
    bound.Y = min(line.start.Y, line.finish.Y);
    bound.width = max(line.start.X, line.finish.X) - bound.X;
    bound.height = max(line.start.Y, line.finish.Y) - bound.Y;

    return bound;
}

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    framebuffer_t *fb = framebuffer_open();

    if (fb == NULL)
    {
        error_print("Failled to open the framebuffer.");
        return -1;
    }

    IOStream *random_device = iostream_open("/dev/random", IOSTREAM_READ);

    if (random_device == NULL)
    {
        error_print("Failled to open random");
        return -1;
    }

    do
    {
        line_t line;
        iostream_read(random_device, &line, sizeof(line));

        line.start.X = abs((int)line.start.X % 800);
        line.start.Y = abs((int)line.start.Y % 600);
        line.finish.X = abs((int)line.finish.X % 800);
        line.finish.Y = abs((int)line.finish.Y % 600);

        painter_draw_line(fb->painter, line.start, line.finish, line.color);

        framebuffer_mark_dirty(fb, line_bound(line));
        framebuffer_blit_dirty(fb);
    } while (true);

    return 0;
}