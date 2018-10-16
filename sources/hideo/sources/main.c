/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

// main.c - The Hideo compositor and window manager.

#include "hideo.h"
#include "hideo-internal.h"

#include <skift/io.h>
#include <skift/drawing.h>

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


    while(1) 
    {
        sk_io_mouse_get_position(&mouse_x, &mouse_y);
        drawing_clear(screen, 0xffff00);
        drawing_fillrect(screen, mouse_x, mouse_y, 16, 16, 0xff);
        sk_io_graphic_blit(screen->buffer);
    }

    return 0;
}
