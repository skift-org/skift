/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* terminal: terminal host process                                            */

#include <skift/iostream.h>
#include <skift/error.h>
#include <skift/logger.h>

#include <hjert/devices/textmode.h>

#include <vtconsole.h>

static iostream_t *textmode_device;
static iostream_t *terminal_fifo;
static textmode_info_t textmode_info;
static ushort *textmode_buffer;

static byte colors[] = {
    [VTCOLOR_BLACK] = TEXTMODE_COLOR_BLACK,
    [VTCOLOR_RED] = TEXTMODE_COLOR_RED,
    [VTCOLOR_GREEN] = TEXTMODE_COLOR_GREEN,
    [VTCOLOR_YELLOW] = TEXTMODE_COLOR_BROWN,
    [VTCOLOR_BLUE] = TEXTMODE_COLOR_BLUE,
    [VTCOLOR_MAGENTA] = TEXTMODE_COLOR_MAGENTA,
    [VTCOLOR_CYAN] = TEXTMODE_COLOR_CYAN,
    [VTCOLOR_GREY] = TEXTMODE_COLOR_LIGHT_GRAY,
};

static byte brightcolors[] = {
    [VTCOLOR_BLACK] = TEXTMODE_COLOR_GRAY,
    [VTCOLOR_RED] = TEXTMODE_COLOR_LIGHT_RED,
    [VTCOLOR_GREEN] = TEXTMODE_COLOR_LIGHT_GREEN,
    [VTCOLOR_YELLOW] = TEXTMODE_COLOR_LIGHT_YELLOW,
    [VTCOLOR_BLUE] = TEXTMODE_COLOR_LIGHT_BLUE,
    [VTCOLOR_MAGENTA] = TEXTMODE_COLOR_LIGHT_MAGENTA,
    [VTCOLOR_CYAN] = TEXTMODE_COLOR_LIGHT_CYAN,
    [VTCOLOR_GREY] = TEXTMODE_COLOR_WHITE,
};

void paint_callback(vtconsole_t *vtc, vtcell_t *vtc_cell, int x, int y)
{
    UNUSED(vtc);

    if (vtc_cell->attr.bright)
    {
        textmode_buffer[x + y * textmode_info.width] = TEXTMODE_ENTRY(vtc_cell->c, brightcolors[vtc_cell->attr.fg], colors[vtc_cell->attr.bg]);
    }
    else
    {
        textmode_buffer[x + y * textmode_info.width] = TEXTMODE_ENTRY(vtc_cell->c, colors[vtc_cell->attr.fg], colors[vtc_cell->attr.bg]);
    }
}

void cursor_move_callback(vtconsole_t *vtc, vtcursor_t *cur)
{
    UNUSED(vtc);

    textmode_info.cursor_x = cur->x;
    textmode_info.cursor_y = cur->y;
}

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    textmode_device = iostream_open(TEXTMODE_DEVICE, IOSTREAM_WRITE);

    if (textmode_device == NULL)
    {
        error_print("Failled to open textmode device file");
        return -1;
    }

    iostream_set_write_buffer_mode(textmode_device, IOSTREAM_BUFFERED_NONE);

    if (iostream_ioctl(textmode_device, TEXTMODE_IOCTL_GET_INFO, &textmode_info))
    {
        error_print("Failled to get textmode info");
        return -1;
    }

    terminal_fifo = iostream_open("/dev/term", IOSTREAM_READ);

    if (terminal_fifo == NULL)
    {
        error_print("Failled to open terminal fifo file");
        return -1;
    }

    iostream_set_read_buffer_mode(terminal_fifo, IOSTREAM_BUFFERED_NONE);

    vtconsole_t *vtc = vtconsole(80, 25, paint_callback, cursor_move_callback);
    textmode_buffer = malloc(textmode_info.width * textmode_info.height * sizeof(ushort));

    var do_exit = false;

    do
    {
        #define READ_BUFFER_SIZE 512
        char buffer[READ_BUFFER_SIZE];
        var size = iostream_read(terminal_fifo, buffer, READ_BUFFER_SIZE);

        vtconsole_write(vtc, buffer, size);

        iostream_ioctl(textmode_device, TEXTMODE_IOCTL_SET_INFO, &textmode_info);
        iostream_write(textmode_device, textmode_buffer, textmode_info.width * textmode_info.height * sizeof(ushort));
    } while (!do_exit);

    vtconsole_delete(vtc);

    return 0;
}
