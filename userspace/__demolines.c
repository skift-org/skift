/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>

#include <libgraphic/Framebuffer.h>

typedef struct
{
    Point start;
    Point finish;
    Color color;
} line_t;

Rectangle line_bound(line_t line)
{
    Rectangle bound;

    bound.X = MIN(line.start.X, line.finish.X);
    bound.Y = MIN(line.start.Y, line.finish.Y);
    bound.width = MAX(line.start.X, line.finish.X) - bound.X;
    bound.height = MAX(line.start.Y, line.finish.Y) - bound.Y;

    return bound;
}

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    Framebuffer *framebuffer = framebuffer_open();

    if (handle_has_error(framebuffer))
    {
        handle_printf_error(framebuffer, "failled to open /dev/framebuffer");
        framebuffer_close(framebuffer);

        return -1;
    }

    Rectangle fb_bound = framebuffer_bound(framebuffer);
    Stream *random_device = stream_open("/dev/random", OPEN_READ);

    if (handle_has_error(random_device))
    {
        handle_printf_error(random_device, "Failled to open /dev/random");
        stream_close(random_device);
    }

    do
    {
        for (int i = 0; i < 100; i++)
        {
            /* code */

            line_t line;
            stream_read(random_device, &line, sizeof(line));

            line.start.X = abs((int)line.start.X % fb_bound.width);
            line.start.Y = abs((int)line.start.Y % fb_bound.height);
            line.finish.X = abs((int)line.finish.X % fb_bound.width);
            line.finish.Y = abs((int)line.finish.Y % fb_bound.height);

            painter_draw_line(framebuffer->painter, line.start, line.finish, line.color);

            framebuffer_mark_dirty(framebuffer, line_bound(line));
        }
        framebuffer_blit_dirty(framebuffer);
    } while (true);

    return 0;
}