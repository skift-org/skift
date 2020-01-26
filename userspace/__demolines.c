/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/error.h>
#include <libsystem/io/Stream.h>

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
    Rectangle fb_bound = framebuffer_bound(fb);

    if (fb == NULL)
    {
        error_print("Failled to open the framebuffer.");
        return -1;
    }

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

            painter_draw_line(fb->painter, line.start, line.finish, line.color);

            framebuffer_mark_dirty(fb, line_bound(line));
        }
        framebuffer_blit_dirty(fb);
    } while (true);

    return 0;
}