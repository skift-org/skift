/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/iostream.h>
#include <libsystem/cstring.h>
#include <libsystem/error.h>
#include <libgraphic/bitmap.h>
#include <libgraphic/painter.h>
#include <libsystem/logger.h>
#include <libsystem/assert.h>

#include <libdevice/framebuffer.h>

framebuffer_mode_info_t mode_info = {true, 800, 600};

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    iostream_t *framebuffer_device = iostream_open(FRAMEBUFFER_DEVICE, IOSTREAM_READ);

    if (framebuffer_device == NULL)
    {
        error_print("Failled to open " FRAMEBUFFER_DEVICE);
        return -1;
    }

    if (iostream_call(framebuffer_device, FRAMEBUFFER_CALL_SET_MODE, &mode_info) < 0)
    {
        error_print("Ioctl to " FRAMEBUFFER_DEVICE " failled");
        return -1;
    }

    bitmap_t *fb = bitmap(800, 600);

    assert(fb);

    painter_t *paint = painter(fb);

    assert(paint);

    float time = 0.0;

    do
    {
        for (int x = 0; x < fb->width; x++)
        {
            for (int y = 0; y < fb->height; y++)
            {
                painter_plot_pixel(paint, (point_t){x, y}, HSV(((float)x / (float)fb->width) * 360.0, ((float)y / (float)fb->height) * 1.0, 1.0));
            }
        }

        time += 0.001;

        iostream_call(framebuffer_device, FRAMEBUFFER_CALL_BLIT, fb->buffer);
    } while (true);

    return 0;
}