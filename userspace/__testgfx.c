/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/Result.h>
#include <libsystem/assert.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>

#include <libgraphic/Framebuffer.h>

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

    Bitmap *test = bitmap_load_from("/res/font/mono.png");

    assert(test);
    logger_info("Image loaded %dx%d", test->width, test->height);

    int frame = 0;

    do
    {
        for (int x = 0; x < framebuffer->width; x++)
        {
            for (int y = 0; y < framebuffer->height; y++)
            {
                painter_plot_pixel(framebuffer->painter, (Point){x, y}, (Color){{x ^ y, x ^ y, x ^ y, 255}});
            }
        }

        painter_blit_bitmap(framebuffer->painter, test, bitmap_bound(test), ((Rectangle){{400 - (frame % 800) / 2, 300 - (frame % 600) / 2, frame % 800, frame % 600}}));

        painter_draw_rectangle(framebuffer->painter, (Rectangle){{75, 75, 100, 100}}, COLOR_BLACK);
        painter_fill_rectangle(framebuffer->painter, (Rectangle){{100, 100, 100, 100}}, ALPHA(COLOR_RED, 0.5));
        painter_fill_rectangle(framebuffer->painter, (Rectangle){{125, 125, 100, 100}}, ALPHA(COLOR_GREEN, 0.5));
        painter_fill_rectangle(framebuffer->painter, (Rectangle){{150, 150, 100, 100}}, ALPHA(COLOR_BLUE, 0.5));

        char message[128];
        snprintf(message, 128, "%d frames", frame++);

        framebuffer_blit(framebuffer);
    } while (true);

    return 0;
}