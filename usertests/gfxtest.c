/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/iostream.h>
#include <skift/cstring.h>
#include <skift/error.h>
#include <skift/bitmap.h>
#include <skift/painter.h>
#include <skift/logger.h>
#include <skift/assert.h>

#include <hjert/devices/framebuffer.h>

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

    if (iostream_ioctl(framebuffer_device, FRAMEBUFFER_IOCTL_SET_MODE, &mode_info) < 0)
    {
        error_print("Ioctl to " FRAMEBUFFER_DEVICE " failled");
        return -1;
    }

    bitmap_t *fb = bitmap(800, 600);
    bitmap_t *test = bitmap_load_from("/res/font/mono.png");

    assert(test);
    logger_log(LOG_INFO, "Image loaded %dx%d", test->width, test->height);

    painter_t *paint = painter(fb);

    int frame = 0;

    do
    {
        for (int x = 0; x < fb->width; x++)
        {
            for (int y = 0; y < fb->height; y++)
            {
                painter_plot_pixel(paint, (point_t){x, y}, (color_t){{x ^ y, x ^ y, x ^ y, 255}});
            }
        }

        painter_blit_bitmap(paint, test, bitmap_bound(test), ((rectangle_t){{400 - (frame % 800) / 2, 300 - (frame % 600) / 2, frame % 800, frame % 600}}));

        painter_draw_rect(paint, (rectangle_t){{75, 75, 100, 100}}, (color_t){{255, 255, 255, 255}});
        painter_fill_rect(paint, (rectangle_t){{100, 100, 100, 100}}, (color_t){{255, 0, 0, 125}});
        painter_fill_rect(paint, (rectangle_t){{125, 125, 100, 100}}, (color_t){{0, 255, 0, 125}});
        painter_fill_rect(paint, (rectangle_t){{150, 150, 100, 100}}, (color_t){{0, 0, 255, 125}});

        char message[128];
        snprintf(message, 128, "%d frames", frame++);

        iostream_ioctl(framebuffer_device, FRAMEBUFFER_IOCTL_BLIT, fb->buffer);
    } while (true);

    return 0;
}