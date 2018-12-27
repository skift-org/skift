/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* graphic.c: graphic device frontend.                                        */

/*
 * TODO:
 * - ADD support for textmod graphics
 */

#include <string.h>
#include <skift/logger.h>

#include "kernel/dev/bga.h"
#include "kernel/memory.h"
#include "kernel/memory.h"

#include "kernel/graphic.h"

uint *physical_framebuffer = NULL;
uint *virtual_framebuffer = NULL;

uint graphic_width = 0;
uint graphic_height = 0;

/* --- Graphic device setup ------------------------------------------------- */

void graphic_early_setup(uint width, uint height)
{
    if (bga_is_available())
    {
        sk_log(LOG_FINE, "Bochs graphics adaptor found!");
        graphic_width = width;
        graphic_height = height;

        bga_mode(width, height);
        physical_framebuffer = (uint *)bga_get_framebuffer();
        virtual_framebuffer = physical_framebuffer;
    }
    else
    {
        sk_log(LOG_WARNING, "No graphic device found!");
        graphic_width = 0;
        graphic_height = 0;
        physical_framebuffer = NULL;
    }
}

void graphic_setup()
{
    if (physical_framebuffer != NULL)
    {
        uint page_count = PAGE_ALIGN(graphic_width * graphic_height * sizeof(uint)) / PAGE_SIZE;
        virtual_framebuffer = (uint *)memory_alloc_at(memory_kpdir(), page_count, (uint)physical_framebuffer, 0);
    }
}

/* --- Screen size ---------------------------------------------------------- */

void graphic_size(uint *width, uint *height)
{
    if (virtual_framebuffer == NULL)
    {
        *width = 0;
        *height = 0; 
    }
    else
    {
        *width = graphic_width;
        *height = graphic_height;
    }
}

/* --- Drawing to the screen ------------------------------------------------ */

void graphic_blit(uint *buffer)
{
    if (virtual_framebuffer != NULL)
        memcpy(virtual_framebuffer, buffer, graphic_width * graphic_height * sizeof(uint));
}

void graphic_blit_region(uint *buffer, uint x, uint y, uint w, uint h)
{
    if (virtual_framebuffer != NULL)
    {
        for (uint xx = 0; xx < w; xx++)
        {
            for (uint yy = 0; yy < h; yy++)
            {
                graphic_pixel(x + xx, y + yy, buffer[(x + xx) + (y + yy) * graphic_width]);
            }
        }
    }
}

inline void graphic_pixel(uint x, uint y, uint color)
{
    if (virtual_framebuffer != NULL && x < graphic_width && y < graphic_height)
        virtual_framebuffer[x + y * graphic_width] = color;
}
