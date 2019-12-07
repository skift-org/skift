/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>
#include <libsystem/error.h>
#include <libsystem/iostream.h>

#include <libgraphic/framebuffer.h>

#define TEXT "The quick brown fox jumps over the lazy dog."

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

    painter_clear(fb->painter, COLOR_BLACK);

    for (int i = 0; fonts[i] != NULL; i++)
    {
        painter_draw_text(fb->painter, fonts[i], TEXT, strlen(TEXT), (Point){16, 32 + 32 * i}, 16, COLOR_WHITE);
    }

    framebuffer_blit(fb);

    for (int i = 0; fonts[i] != NULL; i++)
    {
        object_release(fonts[i]);
    }

    return 0;
}