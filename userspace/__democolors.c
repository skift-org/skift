/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/iostream.h>
#include <libsystem/error.h>
#include <libgraphic/framebuffer.h>

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

    do
    {
        for (int x = 0; x < fb->width; x++)
        {
            for (int y = 0; y < fb->height; y++)
            {
                painter_plot_pixel(fb->painter, (point_t){x, y}, HSV(((float)x / (float)fb->width) * 360.0, ((float)y / (float)fb->height) * 1.0, 1.0));
            }
        }

        framebuffer_blit(fb);
    } while (true);

    return 0;
}