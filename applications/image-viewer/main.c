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
    if (argc == 1)
    {
        stream_printf(err_stream, "ImageViewer: No file to open.\n");
        return -1;
    }

    Bitmap *image = NULL;
    Result result = bitmap_load_from_can_fail(argv[1], &image);

    if (result != SUCCESS)
    {
        stream_printf(err_stream, "ImageViewer: Failled to open bitmap file %s: %s\n", argv[1], result_to_string(result));
        return -1;
    }

    Framebuffer *framebuffer = framebuffer_open();

    if (handle_has_error(framebuffer))
    {
        handle_printf_error(framebuffer, "ImageViewer: Failled to open /dev/framebuffer");
        framebuffer_close(framebuffer);

        return -1;
    }

#define ALPHA_PATTERN_SIZE (8)

    for (int x = 0; x < framebuffer->width; x++)
    {
        for (int y = 0; y < framebuffer->height; y++)
        {
            if (((y / ALPHA_PATTERN_SIZE) * framebuffer->width + (x / ALPHA_PATTERN_SIZE) + (y / ALPHA_PATTERN_SIZE) % 2) % 2 == 0)
            {
                painter_plot_pixel(framebuffer->painter, (Point){x, y}, COLOR_LIGHTGREY);
            }
            else
            {
                painter_plot_pixel(framebuffer->painter, (Point){x, y}, COLOR_WHITE);
            }
        }
    }

    painter_blit_bitmap(framebuffer->painter, image, bitmap_bound(image), framebuffer_bound(framebuffer));
    framebuffer_blit(framebuffer);

    Stream *stream = stream_open("/dev/keyboard", OPEN_READ);
    char c;
    stream_read(stream, &c, sizeof(char));

    return 0;
}