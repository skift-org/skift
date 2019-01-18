/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/logger.h>
#include <skift/drawing.h>
#include <skift/atomic.h>
#include <skift/vtconsole.h>

#include "kernel/dev/vga.h"
#include "kernel/filesystem.h"
#include "kernel/console.h"

vtconsole_t* vtc;

byte colors[] = 
{
    [VTCOLOR_BLACK] = vga_black,
    [VTCOLOR_RED] = vga_red,
    [VTCOLOR_GREEN] = vga_green,
    [VTCOLOR_YELLOW] = vga_brown,
    [VTCOLOR_BLUE] = vga_blue,
    [VTCOLOR_MAGENTA] = vga_magenta,
    [VTCOLOR_CYAN] = vga_cyan,
    [VTCOLOR_GREY] = vga_light_gray,
};

byte brightcolors[] = 
{
    [VTCOLOR_BLACK] = vga_gray,
    [VTCOLOR_RED] = vga_light_red,
    [VTCOLOR_GREEN] = vga_light_green,
    [VTCOLOR_YELLOW] = vga_light_yellow,
    [VTCOLOR_BLUE] = vga_light_blue,
    [VTCOLOR_MAGENTA] = vga_light_magenta,
    [VTCOLOR_CYAN] = vga_light_cyan,
    [VTCOLOR_GREY] = vga_white,
};

void console_paint(vtconsole_t* vtc, vtcell_t* cell, int x, int y)
{
    UNUSED(vtc);

    if (cell->attr.bright)
    {
        vga_cell(x, y, vga_entry(cell->c, colors[cell->attr.fg], colors[cell->attr.bg]));
    }
    else
    {
        vga_cell(x, y, vga_entry(cell->c, brightcolors[cell->attr.fg], colors[cell->attr.bg]));
    }
}

void console_cursor_move(vtconsole_t* vtc, vtcursor_t* cur)
{
    UNUSED(vtc);
    UNUSED(cur);
    //vga_cursor(cur->x, cur->y);
}

void console_setup(void)
{
    vtc = vtconsole(vga_screen_width, vga_screen_height, console_paint, console_cursor_move);
}

void console_print(const char *s)
{
    sk_atomic_begin();

    if (vtc != NULL)
    {
        vtconsole_write(vtc, s);
    }

    sk_atomic_end();
}

void console_putchar(char c)
{
    sk_atomic_begin();

    if (vtc != NULL)
    {
        vtconsole_putchar(vtc, c);
    }

    sk_atomic_end();
}
