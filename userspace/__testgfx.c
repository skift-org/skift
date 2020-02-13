/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/assert.h>
#include <libsystem/cstring.h>
#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>

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

    Bitmap *test = bitmap_load_from("/res/font/mono.png");

    assert(test);
    logger_info("Image loaded %dx%d", test->width, test->height);

    int frame = 0;

    do
    {
        for (int x = 0; x < fb->width; x++)
        {
            for (int y = 0; y < fb->height; y++)
            {
                painter_plot_pixel(fb->painter, (Point){x, y}, (Color){{x ^ y, x ^ y, x ^ y, 255}});
            }
        }

        painter_blit_bitmap(fb->painter, test, bitmap_bound(test), ((Rectangle){{400 - (frame % 800) / 2, 300 - (frame % 600) / 2, frame % 800, frame % 600}}));

        painter_draw_rect(fb->painter, (Rectangle){{75, 75, 100, 100}}, (Color){{255, 255, 255, 255}});
        painter_fill_rect(fb->painter, (Rectangle){{100, 100, 100, 100}}, (Color){{255, 0, 0, 125}});
        painter_fill_rect(fb->painter, (Rectangle){{125, 125, 100, 100}}, (Color){{0, 255, 0, 125}});
        painter_fill_rect(fb->painter, (Rectangle){{150, 150, 100, 100}}, (Color){{0, 0, 255, 125}});

        char message[128];
        snprintf(message, 128, "%d frames", frame++);

        framebuffer_blit(fb);
    } while (true);

    return 0;
}