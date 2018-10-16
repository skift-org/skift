/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

// main.c - The Hideo compositor and window manager.

#include <math.h>
#include <skift/io.h>
#include <skift/drawing.h>

#include "hideo.h"
#include "hideo-internal.h"

void window_decorate(bitmap_t * screen, int x, int y, int w, int h)
{
    drawing_fillrect(screen, x, y, w, h, 0xcfcfcf);
    drawing_fillrect(screen, x, y, w, 32, 0xeeeeee);
}

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    sk_io_print("Hideo compositor and window manager");
    uint width, height = 0;

    sk_io_graphic_size(&width, &height);
    bitmap_t* screen = bitmap_ctor(width, height);

    int mouse_x;
    int mouse_y;

    drawing_clear(screen, 0xffffff);

    while(1) 
    {
        sk_io_mouse_get_position(&mouse_x, &mouse_y);

        mouse_x = max(min(mouse_x, (int)width - 1), 0);
        mouse_y = max(min(mouse_y, (int)height - 1), 0);

        sk_io_mouse_set_position(mouse_x, mouse_y);


        window_decorate(screen, 67, 98, 300, 200);

        //drawing_rect(screen, mouse_x, mouse_y, 32, 32, 1, 0xff);
        drawing_line(screen, mouse_x, mouse_y, mouse_x, mouse_y+24, 2, 0x0);
        drawing_line(screen, mouse_x, mouse_y, mouse_x+16, mouse_y+16, 2, 0x0);
        sk_io_graphic_blit(screen->buffer);
    }

    return 0;
}
