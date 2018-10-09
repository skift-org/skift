/* Copyright © 2018 MAKER.                                                    */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

// main.c - The Hideo compositor and window manager.

#include "hideo.h"
#include "hideo-internal.h"

#include "skift/io.h"
#include "libgfx.h"

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    sk_io_print("Hideo compositor and window manager");
    uint width, height = 0;

    sk_io_graphic_size(&width, &height);

    bitmap_t* screen = bitmap_ctor(width, height);
    libgfx_clear(screen, 0xff);
    sk_io_graphic_blit(screen->buffer);

    while(1) 
    {

    }

    return 0;
}
