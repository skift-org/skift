/* conhost.h: console host process                                            */

#include <skift/iostream.h>
#include <skift/error.h>
#include <skift/logger.h>

#include <hjert/devices/textmode.h>

#include <vtconsole.h>

static iostream_t *textmode_device;
static iostream_t *console_fifo;
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

void hexDump(iostream_t *stream, char *desc, void *addr, int len)
{
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char *)addr;

    // Output description if given.
    if (desc != NULL)
        iostream_printf(stream, "%s:\n", desc);

    // Process every byte in the data.
    for (i = 0; i < len; i++)
    {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0)
        {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                iostream_printf(stream, "  %s\n", buff);

            // Output the offset.
            iostream_printf(stream, "  %04x ", i);
        }

        // Now the hex code for the specific character.
        iostream_printf(stream, " %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
        {
            buff[i % 16] = '.';
        }
        else
        {
            buff[i % 16] = pc[i];
        }

        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0)
    {
        iostream_printf(stream, "   ");
        i++;
    }

    // And print the final ASCII bit.
    iostream_printf(stream, "  %s\n", buff);
}

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    textmode_device = iostream_open("/dev/textmode", IOSTREAM_WRITE);

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

    console_fifo = iostream_open("/dev/console", IOSTREAM_READ);

    if (console_fifo == NULL)
    {
        error_print("Failled to open console fifo file");
        return -1;
    }

    iostream_set_read_buffer_mode(console_fifo, IOSTREAM_BUFFERED_NONE);

    vtconsole_t *vtc = vtconsole(80, 25, paint_callback, cursor_move_callback);
    textmode_buffer = malloc(textmode_info.width * textmode_info.height * sizeof(ushort));

    var do_exit = false;

    do
    {
        char buffer[512];
        var size = iostream_read(console_fifo, buffer, 512);

        logger_log(LOG_DEBUG, "Readded %d", size);
        hexDump(log_stream, "What got readed: ", buffer, size);

        vtconsole_write(vtc, buffer, size);

        iostream_ioctl(textmode_device, TEXTMODE_IOCTL_SET_INFO, &textmode_info);
        iostream_write(textmode_device, textmode_buffer, textmode_info.width * textmode_info.height * sizeof(ushort));
    } while (!do_exit);

    vtconsole_delete(vtc);

    return 0;
}
