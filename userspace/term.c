/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* terminal: terminal host process                                            */

#include <libdevice/textmode.h>
#include <libgraphic/framebuffer.h>
#include <libsystem/debug.h>
#include <libsystem/error.h>
#include <libsystem/iostream.h>
#include <libsystem/logger.h>
#include <libterminal/Terminal.h>

static IOStream *terminal_fifo = NULL;

/* --- Textmode terminal ---------------------------------------------------- */

static IOStream *textmode_device = NULL;
static textmode_info_t textmode_info;
static ushort *textmode_buffer = NULL;

static byte textmode_colors[] = {
    [TERMINAL_COLOR_BLACK] = TEXTMODE_COLOR_BLACK,
    [TERMINAL_COLOR_RED] = TEXTMODE_COLOR_RED,
    [TERMINAL_COLOR_GREEN] = TEXTMODE_COLOR_GREEN,
    [TERMINAL_COLOR_YELLOW] = TEXTMODE_COLOR_BROWN,
    [TERMINAL_COLOR_BLUE] = TEXTMODE_COLOR_BLUE,
    [TERMINAL_COLOR_MAGENTA] = TEXTMODE_COLOR_MAGENTA,
    [TERMINAL_COLOR_CYAN] = TEXTMODE_COLOR_CYAN,
    [TERMINAL_COLOR_GREY] = TEXTMODE_COLOR_LIGHT_GRAY,

    [TERMINAL_COLOR_BRIGHT_BLACK] = TEXTMODE_COLOR_GRAY,
    [TERMINAL_COLOR_BRIGHT_RED] = TEXTMODE_COLOR_LIGHT_RED,
    [TERMINAL_COLOR_BRIGHT_GREEN] = TEXTMODE_COLOR_LIGHT_GREEN,
    [TERMINAL_COLOR_BRIGHT_YELLOW] = TEXTMODE_COLOR_LIGHT_YELLOW,
    [TERMINAL_COLOR_BRIGHT_BLUE] = TEXTMODE_COLOR_LIGHT_BLUE,
    [TERMINAL_COLOR_BRIGHT_MAGENTA] = TEXTMODE_COLOR_LIGHT_MAGENTA,
    [TERMINAL_COLOR_BRIGHT_CYAN] = TEXTMODE_COLOR_LIGHT_CYAN,
    [TERMINAL_COLOR_BRIGHT_GREY] = TEXTMODE_COLOR_WHITE,
};

void textmode_TerminalPaintCallback(Terminal *terminal, int x, int y, TerminalCell cell)
{
    textmode_buffer[x + y * terminal->width] = TEXTMODE_ENTRY(
        codepoint_to_cp437(cell.codepoint),
        textmode_colors[cell.attributes.foreground],
        textmode_colors[cell.attributes.background]);
}

void textmode_TerminalCursorCallback(Terminal *terminal, TerminalCursor cursor)
{
    __unused(terminal);

    textmode_info.cursor_x = cursor.x;
    textmode_info.cursor_y = cursor.y;
}

Terminal *terminal_create_textmode_console(void)
{
    textmode_device = iostream_open(TEXTMODE_DEVICE, OPEN_WRITE);

    if (textmode_device == NULL)
    {
        return NULL;
    }

    if (iostream_call(textmode_device, TEXTMODE_CALL_GET_INFO, &textmode_info))
    {
        error_print("Failled to get textmode info");
        return NULL;
    }

    textmode_buffer = malloc(textmode_info.width * textmode_info.height * sizeof(ushort));
    Terminal *terminal = terminal_create(80, 25, textmode_TerminalPaintCallback, textmode_TerminalCursorCallback);

    return terminal;
}

/* --- Framebuffer terminal ------------------------------------------------- */

Point char_size = (Point){9, 24};

static Font *mono_font = NULL;

static framebuffer_t *framebuffer;
static Point framebuffer_cursor = point_zero;

static Color framebuffer_colors[] = {
    [TERMINAL_COLOR_BLACK] = COLOR(0x212121),
    [TERMINAL_COLOR_RED] = COLOR(0xb71c1c),
    [TERMINAL_COLOR_GREEN] = COLOR(0x1b5e20),
    [TERMINAL_COLOR_YELLOW] = COLOR(0xf57f17),
    [TERMINAL_COLOR_BLUE] = COLOR(0x0d47a1),
    [TERMINAL_COLOR_MAGENTA] = COLOR(0x880e4f),
    [TERMINAL_COLOR_CYAN] = COLOR(0x006064),
    [TERMINAL_COLOR_GREY] = COLOR(0x607d8b),

    [TERMINAL_COLOR_BRIGHT_BLACK] = COLOR(0x9e9e9e),
    [TERMINAL_COLOR_BRIGHT_RED] = COLOR(0xf44336),
    [TERMINAL_COLOR_BRIGHT_GREEN] = COLOR(0x4caf50),
    [TERMINAL_COLOR_BRIGHT_YELLOW] = COLOR(0xffeb3b),
    [TERMINAL_COLOR_BRIGHT_BLUE] = COLOR(0x2196f3),
    [TERMINAL_COLOR_BRIGHT_MAGENTA] = COLOR(0xe91e63),
    [TERMINAL_COLOR_BRIGHT_CYAN] = COLOR(0x00bcd4),
    [TERMINAL_COLOR_BRIGHT_GREY] = COLOR(0xeceff1),
};

void framebuffer_TerminalCursorCallback(Terminal *terminal, TerminalCursor cursor)
{
    __unused(terminal);

    framebuffer_cursor = (Point){cursor.x, cursor.y};
}

Terminal *terminal_create_framebuffer_console(void)
{
    logger_info("Creating graphic context...");

    framebuffer = framebuffer_open();

    if (framebuffer == NULL)
    {
        return NULL;
    }

    painter_clear(framebuffer->painter, framebuffer_colors[TERMINAL_COLOR_BLACK]);

    framebuffer_blit(framebuffer);

    mono_font = font_create("mono");

    return terminal_create(framebuffer->width / char_size.X, framebuffer->height / char_size.Y, NULL, framebuffer_TerminalCursorCallback);
}

void paint_repaint_dirty(Terminal *console, Painter *paint)
{
    int repainted_cell = 0;

    for (int y = 0; y < console->height; y++)
    {
        for (int x = 0; x < console->width; x++)
        {
            TerminalCell cell = terminal_cell_at(console, x, y);

            if (cell.dirty)
            {
                repainted_cell++;

                Color background_color = framebuffer_colors[cell.attributes.background];
                Color foreground_color = framebuffer_colors[cell.attributes.foreground];

                Point pos = (Point){x * char_size.X, y * (int)(char_size.Y)};
                Point siz = (Point){char_size.X, (char_size.Y)};

                Rectangle cell_bound;
                cell_bound.position = pos;
                cell_bound.size = siz;

                painter_clear_rect(paint, cell_bound, background_color);

                if (cell.codepoint != U' ')
                {
                    painter_draw_glyph(
                        paint,
                        mono_font,
                        font_glyph(mono_font, cell.codepoint),
                        point_add(pos, (Point){0, 16}),
                        16,
                        foreground_color);
                }

                framebuffer_mark_dirty(framebuffer, cell_bound);

                terminal_cell_undirty(console, x, y);
            }
        }
    }

    framebuffer_blit_dirty(framebuffer);
}

/* --- Terminal read/write loop --------------------------------------------- */

int main(int argc, char const *argv[])
{
    __unused(argc);
    __unused(argv);

    terminal_fifo = iostream_open("/dev/term", OPEN_READ);

    if (terminal_fifo == NULL)
    {
        error_print("Failled to open terminal fifo file");
        return -1;
    }

    Terminal *terminal = NULL;
    bool is_framebuffer = true;
    terminal = terminal_create_framebuffer_console();

    if (terminal == NULL)
    {
        terminal = terminal_create_textmode_console();

        if (terminal == NULL)
        {
            iostream_close(terminal_fifo);

            return -1;
        }
        else
        {
            is_framebuffer = false;
        }
    }

    bool do_exit = false;

    do
    {
#define READ_BUFFER_SIZE 512
        char buffer[READ_BUFFER_SIZE];
        int size = iostream_read(terminal_fifo, buffer, READ_BUFFER_SIZE);

        terminal_write(terminal, buffer, size);

        if (!is_framebuffer)
        {
            iostream_call(textmode_device, TEXTMODE_CALL_SET_INFO, &textmode_info);
            iostream_write(textmode_device, textmode_buffer, textmode_info.width * textmode_info.height * sizeof(ushort));
        }
        else
        {
            paint_repaint_dirty(terminal, framebuffer->painter);
        }
    } while (!do_exit);

    if (terminal)
    {
        terminal_destroy(terminal);
    }

    if (mono_font)
    {
        font_destroy(mono_font);
    }

    return 0;
}
