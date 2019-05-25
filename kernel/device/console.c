/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/cstring.h>

#include <skift/logger.h>
#include <skift/atomic.h>
#include <vtconsole.h>

#include <hjert/dev/vga.h>
#include <hjert/filesystem.h>
#include <hjert/console.h>

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

static void paint_callback(vtconsole_t *vtc, vtcell_t *cell, int x, int y)
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

static void cursor_move_callback(vtconsole_t *vtc, vtcursor_t *cur)
{
    UNUSED(vtc);
    UNUSED(cur);
    vga_cursor(cur->x, cur->y);
}

/* --- Device driver -------------------------------------------------------- */

static int console_device_read(stream_t* stream, void* buffer, uint size)
{
    UNUSED(stream);
    UNUSED(buffer);
    UNUSED(size);

    return -1;
}

static int console_device_write(stream_t* stream, const void* buffer, uint size)
{
    UNUSED(stream);

    vtconsole_write(vtc, buffer, size);
    
    return size;
}

/* --- Public functions ----------------------------------------------------- */

void console_setup(void)
{
    device_t console_device = 
    {
        .read = console_device_read,
        .write = console_device_write,
    };

    vtc = vtconsole(VGA_SCREEN_WIDTH, VGA_SCREEN_HEIGHT, paint_callback, cursor_move_callback);
    
    FILESYSTEM_MKDEV("console", console_device);
}

void console_print(const char *s)
{
    atomic_begin();

    if (vtc != NULL)
    {
        vtconsole_write(vtc, s, strlen(s));
    }

    atomic_end();
}

void console_putchar(char c)
{
    atomic_begin();

    if (vtc != NULL)
    {
        vtconsole_putchar(vtc, c);
    }

    atomic_end();
}
