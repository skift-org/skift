/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libgraphic/framebuffer.h>
#include <libsystem/error.h>
#include <libsystem/io/Stream.h>

int main(int argc, char **argv)
{
    __unused(argc);
    __unused(argv);

    framebuffer_t *framebuffer = framebuffer_open();

    if (framebuffer == NULL)
    {
        error_print("Failled to open the framebuffer.");
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