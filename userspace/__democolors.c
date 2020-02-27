/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/Framebuffer.h>
#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>

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

    do
    {
        for (int x = 0; x < framebuffer->width; x++)
        {
            for (int y = 0; y < framebuffer->height; y++)
            {
                Color color = HSV((x / (float)framebuffer->width) * 360.0, (y / (float)framebuffer->height), 1.0);
                painter_plot_pixel(framebuffer->painter, (Point){x, y}, color);
            }
        }

        framebuffer_blit(framebuffer);
    } while (true);

    return 0;
}