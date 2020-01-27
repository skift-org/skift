/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* terminal: terminal host process                                            */

#include <libdevice/textmode.h>
#include <libgraphic/framebuffer.h>
#include <libsystem/debug.h>
#include <libsystem/error.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>
#include <libsystem/process/Launchpad.h>
#include <libterminal/Terminal.h>

static Stream *terminal_fifo = NULL;

/* --- Textmode terminal ---------------------------------------------------- */

static Stream *textmode_device = NULL;
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
    if (cell.attributes.inverted)
    {
        textmode_buffer[x + y * terminal->width] = TEXTMODE_ENTRY(
            codepoint_to_cp437(cell.codepoint),
            textmode_colors[cell.attributes.background],
            textmode_colors[cell.attributes.foreground]);
    }
    else
    {
        textmode_buffer[x + y * terminal->width] = TEXTMODE_ENTRY(
            codepoint_to_cp437(cell.codepoint),
            textmode_colors[cell.attributes.foreground],
            textmode_colors[cell.attributes.background]);
    }
}

void textmode_TerminalCursorCallback(Terminal *terminal, TerminalCursor cursor)
{
    __unused(terminal);

    textmode_info.cursor_x = cursor.x;
    textmode_info.cursor_y = cursor.y;
}

Terminal *terminal_create_textmode_console(void)
{
    textmode_device = stream_open(TEXTMODE_DEVICE, OPEN_WRITE);

    if (handle_has_error(textmode_device))
    {
        stream_close(textmode_device);
        textmode_device = NULL;
        return NULL;
    }

    if (stream_call(textmode_device, TEXTMODE_CALL_GET_INFO, &textmode_info) != ERR_SUCCESS)
    {
        stream_close(textmode_device);
        textmode_device = NULL;
        return NULL;
    }

    textmode_buffer = malloc(textmode_info.width * textmode_info.height * sizeof(ushort));
    Terminal *terminal = terminal_create(80, 25, textmode_TerminalPaintCallback, textmode_TerminalCursorCallback);

    return terminal;
}

/* --- Framebuffer terminal ------------------------------------------------- */

Point char_size = (Point){7, 16};

static Font *mono_font = NULL;

static framebuffer_t *framebuffer;
static Point framebuffer_cursor = point_zero;

static Color framebuffer_colors[] = {
    [TERMINAL_COLOR_BLACK] = COLOR(0x0A0E14),
    [TERMINAL_COLOR_RED] = COLOR(0xff3333),
    [TERMINAL_COLOR_GREEN] = COLOR(0xb8cc52),
    [TERMINAL_COLOR_YELLOW] = COLOR(0xe6c446),
    [TERMINAL_COLOR_BLUE] = COLOR(0x36a3d9),
    [TERMINAL_COLOR_MAGENTA] = COLOR(0xf07078),
    [TERMINAL_COLOR_CYAN] = COLOR(0x95e5cb),
    [TERMINAL_COLOR_GREY] = COLOR(0xffffff),
    [TERMINAL_COLOR_BRIGHT_BLACK] = COLOR(0x323232),
    [TERMINAL_COLOR_BRIGHT_RED] = COLOR(0xff6565),
    [TERMINAL_COLOR_BRIGHT_GREEN] = COLOR(0xe9fe83),
    [TERMINAL_COLOR_BRIGHT_YELLOW] = COLOR(0xfff778),
    [TERMINAL_COLOR_BRIGHT_BLUE] = COLOR(0x68d4ff),
    [TERMINAL_COLOR_BRIGHT_MAGENTA] = COLOR(0xffa3aa),
    [TERMINAL_COLOR_BRIGHT_CYAN] = COLOR(0xc7fffc),
    [TERMINAL_COLOR_BRIGHT_GREY] = COLOR(0xffffff),
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

                if (cell.attributes.inverted)
                {
                    background_color = framebuffer_colors[cell.attributes.foreground];
                    foreground_color = framebuffer_colors[cell.attributes.background];
                }

                Point pos = (Point){x * char_size.X, y * (int)(char_size.Y)};
                Point siz = (Point){char_size.X, (char_size.Y)};

                Rectangle cell_bound;
                cell_bound.position = pos;
                cell_bound.size = siz;

                painter_clear_rect(paint, cell_bound, background_color);

                if (cell.codepoint != U' ')
                {
                    Glyph *glyph = font_glyph(mono_font, cell.codepoint);

                    if (glyph != NULL)
                    {
                        painter_draw_glyph(
                            paint,
                            mono_font,
                            glyph,
                            point_add(pos, (Point){0, 12}),
                            foreground_color);

                        if (cell.attributes.bold)
                        {
                            painter_draw_glyph(
                                paint,
                                mono_font,
                                glyph,
                                point_add(pos, (Point){1, 12}),
                                foreground_color);
                        }

                        if (cell.attributes.underline)
                        {
                            painter_draw_line(paint, point_add(pos, (Point){0, 13}), point_add(pos, (Point){cell_bound.width, 13}), foreground_color);
                        }
                    }
                    else
                    {
                        painter_draw_rect(paint, cell_bound, foreground_color);
                    }
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

    terminal_fifo = stream_open("/dev/term", OPEN_READ | OPEN_WRITE);

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
            stream_close(terminal_fifo);

            return -1;
        }
        else
        {
            is_framebuffer = false;
        }
    }

    Launchpad *shell_launchpad = launchpad_create("sh", "/bin/sh");
    launchpad_handle(shell_launchpad, HANDLE(terminal_fifo), 1);
    launchpad_handle(shell_launchpad, HANDLE(terminal_fifo), 2);
    launchpad_launch(shell_launchpad);

    bool do_exit = false;

    do
    {
#define READ_BUFFER_SIZE 512
        char buffer[READ_BUFFER_SIZE];
        int size = stream_read(terminal_fifo, buffer, READ_BUFFER_SIZE);

        terminal_write(terminal, buffer, size);

        if (!is_framebuffer)
        {
            stream_call(textmode_device, TEXTMODE_CALL_SET_INFO, &textmode_info);
            stream_write(textmode_device, textmode_buffer, textmode_info.width * textmode_info.height * sizeof(ushort));
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
