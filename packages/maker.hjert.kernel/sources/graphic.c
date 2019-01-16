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
#include "kernel/dev/vga.h"
#include "kernel/memory.h"
#include "kernel/graphic.h"

typedef enum
{
    GFXDEV_VGA,
    GFXDEV_BGA,
} graphic_device_t;

static int COLOR24[] =
{
    [TMCOLOR_BLACK] = 0x1D1F21,
    [TMCOLOR_RED] = 0xA54242,
    [TMCOLOR_GREEN] = 0x8C9440,
    [TMCOLOR_YELLOW] = 0xDE935F,
    [TMCOLOR_BLUE] = 0x5F819D,
    [TMCOLOR_MAGENTA] = 0x85678F,
    [TMCOLOR_CYAN] = 0x5E8D87,
    [TMCOLOR_LIGHT_GREY] = 0x707880,

    [TMCOLOR_DARK_GREY] = 0x373B41,
    [TMCOLOR_LIGHT_RED] = 0xCC6666,
    [TMCOLOR_LIGHT_GREEN] = 0xB5BD68,
    [TMCOLOR_LIGHT_YELLOW] = 0xF0C674,
    [TMCOLOR_LIGHT_BLUE] = 0x81A2BE,
    [TMCOLOR_LIGHT_MAGENTA] = 0xB294BB,
    [TMCOLOR_LIGHT_CYAN] = 0x8ABEB7,
    [TMCOLOR_WHITE] = 0xC5C8C6,
};

static int COLORVGA[] =
{
    [TMCOLOR_BLACK]         = vga_black,
    [TMCOLOR_RED]           = vga_red,
    [TMCOLOR_GREEN]         = vga_green,
    [TMCOLOR_YELLOW]        = vga_brown,
    [TMCOLOR_BLUE]          = vga_blue,
    [TMCOLOR_MAGENTA]       = vga_magenta,
    [TMCOLOR_CYAN]          = vga_cyan,
    [TMCOLOR_LIGHT_GREY]    = vga_light_gray,

    [TMCOLOR_DARK_GREY]     = vga_gray,
    [TMCOLOR_LIGHT_RED]     = vga_light_red,
    [TMCOLOR_LIGHT_GREEN]   = vga_light_green,
    [TMCOLOR_LIGHT_YELLOW]  = vga_light_yellow,
    [TMCOLOR_LIGHT_BLUE]    = vga_light_blue,
    [TMCOLOR_LIGHT_MAGENTA] = vga_light_magenta,
    [TMCOLOR_LIGHT_CYAN]    = vga_light_cyan,
    [TMCOLOR_WHITE]         = vga_white,
};

static uint *physical_framebuffer = NULL;
static uint *virtual_framebuffer = NULL;

static uint graphic_width = 0;
static uint graphic_height = 0;

static uint textmode_width = vga_screen_width;
static uint textmode_height = vga_screen_height;

static graphic_device_t device = GFXDEV_VGA;

/* --- Graphic device setup ------------------------------------------------- */
void graphic_early_setup(uint width, uint height)
{
    if (bga_is_available() && false)
    {
        sk_log(LOG_FINE, "Bochs graphics adaptor found!");

        graphic_width = width;
        graphic_height = height;

        textmode_width = graphic_width / 8;
        textmode_height = graphic_height / 16;

        bga_mode(width, height);
        physical_framebuffer = (uint *)bga_get_framebuffer();
        virtual_framebuffer = physical_framebuffer;

        device = GFXDEV_BGA;
    }
    else
    {
        sk_log(LOG_WARNING, "No graphic device found!");
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
    if (device == GFXDEV_BGA && virtual_framebuffer != NULL)
        memcpy(virtual_framebuffer, buffer, graphic_width * graphic_height * sizeof(uint));
}

void graphic_blit_region(uint *buffer, uint x, uint y, uint w, uint h)
{
    if (device == GFXDEV_BGA && virtual_framebuffer != NULL)
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

inline void graphic_char(uint x, uint y, char c, textmode_color_t fg, textmode_color_t bg)
{
    if (x < textmode_width && y < textmode_height)
    {
        if (device == GFXDEV_VGA)
        {
            vga_cell(x, y, vga_entry(c, COLORVGA[fg], COLORVGA[bg]));
        }
        else if (device == GFXDEV_BGA)
        {
            // TODO
        }
    }
}

inline void graphic_pixel(uint x, uint y, uint color)
{
    if (virtual_framebuffer != NULL && x < graphic_width && y < graphic_height)
        virtual_framebuffer[x + y * graphic_width] = color;
}
