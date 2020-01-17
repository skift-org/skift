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

    framebuffer_t *framebuffer = framebuffer_open();

    if (framebuffer == NULL)
    {
        error_print("Failled to open the framebuffer.");
        return -1;
    }

    Font *fonts[] = {
        font_create("mono"),
        font_create("mono-italic"),
        font_create("mono-bold"),
        font_create("mono-bold-italic"),
        font_create("sans"),
        font_create("sans-italic"),
        font_create("sans-bold"),
        font_create("sans-bold-italic"),
        font_create("serif"),
        font_create("serif-italic"),
        font_create("serif-bold"),
        font_create("serif-bold-italic"),
        NULL,
    };

    painter_clear(framebuffer->painter, COLOR_BLACK);

    for (int i = 0; fonts[i] != NULL; i++)
    {
        painter_draw_text(framebuffer->painter, fonts[i], TEXT, strlen(TEXT), (Point){16, 32 + 32 * i}, 16, COLOR_WHITE);
    }

    framebuffer_blit(framebuffer);

    for (int i = 0; fonts[i] != NULL; i++)
    {
        font_destroy(fonts[i]);
    }

    return 0;
}