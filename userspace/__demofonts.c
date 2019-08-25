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

    assert(framebuffer_device);

    assert(iostream_ioctl(framebuffer_device, FRAMEBUFFER_IOCTL_SET_MODE, &mode_info) >= 0);

    bitmap_t *fb = bitmap(800, 600);

    assert(fb);

    font_t *fonts[] = {
        font("mono"),
        font("mono-italic"),
        font("mono-bold"),
        font("mono-bold-italic"),
        font("sans"),
        font("sans-italic"),
        font("sans-bold"),
        font("sans-bold-italic"),
        font("serif"),
        font("serif-italic"),
        font("serif-bold"),
        font("serif-bold-italic"),
        NULL,
    };

    painter_t *paint = painter(fb);

    assert(paint);

    painter_fill_rect(paint, bitmap_bound(fb), COLOR_BLACK);
    painter_fill_rect(paint, (rectangle_t){{16, 16, 16, 16}}, COLOR_RED);

    const char* text = "The quick brown fox jumps over the lazy dog.";

    for (int i = 0; fonts[i] != NULL; i++)
    {
        painter_draw_text(paint, fonts[i], text, strlen(text), (point_t){16, 32 + 32 * i}, 16, COLOR_WHITE);
    }
    

    iostream_ioctl(framebuffer_device, FRAMEBUFFER_IOCTL_BLIT, fb->buffer);

    for (int i = 0; fonts[i] != NULL; i++)
    {
        object_release(fonts[i]);
    }

    return 0;
}