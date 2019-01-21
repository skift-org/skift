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

static vtconsole_t *vtc;

static byte colors[] =
{
    [VTCOLOR_BLACK] = VGACOLOR_BLACK,
    [VTCOLOR_RED] = VGACOLOR_RED,
    [VTCOLOR_GREEN] = VGACOLOR_GREEN,
    [VTCOLOR_YELLOW] = VGACOLOR_BROWN,
    [VTCOLOR_BLUE] = VGACOLOR_BLUE,
    [VTCOLOR_MAGENTA] = VGACOLOR_MAGENTA,
    [VTCOLOR_CYAN] = VGACOLOR_CYAN,
    [VTCOLOR_GREY] = VGACOLOR_LIGHT_GRAY,
};

static byte brightcolors[] =
{
    [VTCOLOR_BLACK] = VGACOLOR_GRAY,
    [VTCOLOR_RED] = VGACOLOR_LIGHT_RED,
    [VTCOLOR_GREEN] = VGACOLOR_LIGHT_GREEN,
    [VTCOLOR_YELLOW] = VGACOLOR_LIGHT_YELLOW,
    [VTCOLOR_BLUE] = VGACOLOR_LIGHT_BLUE,
    [VTCOLOR_MAGENTA] = VGACOLOR_LIGHT_MAGENTA,
    [VTCOLOR_CYAN] = VGACOLOR_LIGHT_CYAN,
    [VTCOLOR_GREY] = VGACOLOR_WHITE,
};

void console_paint(vtconsole_t *vtc, vtcell_t *cell, int x, int y)
{
    UNUSED(vtc);

    if (cell->attr.bright)
    {
        vga_cell(x, y, VGA_ENTRY(cell->c, brightcolors[cell->attr.fg], colors[cell->attr.bg]));
    }
    else
    {
        vga_cell(x, y, VGA_ENTRY(cell->c, colors[cell->attr.fg], colors[cell->attr.bg]));
    }
}

void console_cursor_move(vtconsole_t *vtc, vtcursor_t *cur)
{
    UNUSED(vtc);
    UNUSED(cur);
    vga_cursor(cur->x, cur->y);
}

void console_setup(void)
{
    vtc = vtconsole(VGA_SCREEN_WIDTH, VGA_SCREEN_HEIGHT, console_paint, console_cursor_move);
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
