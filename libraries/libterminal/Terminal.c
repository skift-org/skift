/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libmath/MinMax.h>
#include <libsystem/assert.h>
#include <libterminal/Terminal.h>

static void terminal_decoder_callback(Codepoint codepoint, Terminal *terminal)
{
    terminal_write_codepoint(terminal, codepoint);
}

Terminal *terminal_create(int width, int height, TerminalRenderer *renderer)
{
    Terminal *terminal = __create(Terminal);

    terminal->width = width;
    terminal->height = height;
    terminal->buffer = calloc(width * height, sizeof(TerminalCell));

    terminal->decoder = utf8decoder_create((UTF8DecoderCallback)terminal_decoder_callback, terminal);
    terminal->renderer = renderer;

    terminal->cursor = (TerminalCursor){0, 0, true};
    terminal->saved_cursor = (TerminalCursor){0, 0, true};

    terminal->current_attributes = (TerminalAttributes){TERMINAL_COLOR_DEFAULT_FOREGROUND, TERMINAL_COLOR_DEFAULT_BACKGROUND, false, false, false};
    terminal->default_attributes = (TerminalAttributes){TERMINAL_COLOR_DEFAULT_FOREGROUND, TERMINAL_COLOR_DEFAULT_BACKGROUND, false, false, false};

    terminal->parameters_top = 0;

    for (int i = 0; i < TERMINAL_MAX_PARAMETERS; i++)
    {
        terminal->parameters[i].empty = true;
        terminal->parameters[i].value = 0;
    }

    terminal_clear_all(terminal);

    return terminal;
}

void terminal_destroy(Terminal *terminal)
{
    utf8decoder_destroy(terminal->decoder);
    terminal_renderer_destroy(terminal->renderer);
    free(terminal->buffer);
    free(terminal);
}

void terminal_clear(Terminal *terminal, int fromx, int fromy, int tox, int toy)
{
    for (int i = fromx + fromy * terminal->width; i < tox + toy * terminal->width; i++)
    {
        terminal_set_cell(
            terminal,
            i % terminal->width,
            i / terminal->width,
            (TerminalCell){U' ', terminal->current_attributes, true});
    }
}

void terminal_clear_all(Terminal *terminal)
{
    terminal_clear(terminal, 0, 0, terminal->width, terminal->height);
}

void terminal_clear_line(Terminal *terminal, int line)
{
    if (line >= 0 && line < terminal->height)
    {
        for (int i = 0; i < terminal->width; i++)
        {
            terminal_set_cell(
                terminal,
                i,
                line,
                (TerminalCell){U' ', terminal->current_attributes, true});
        }
    }
}

TerminalCell terminal_cell_at(Terminal *terminal, int x, int y)
{
    if (x >= 0 && x < terminal->width &&
        y >= 0 && y < terminal->height)
    {
        return terminal->buffer[y * terminal->width + x];
    }

    ASSERT_NOT_REACHED();
}

void terminal_cell_undirty(Terminal *terminal, int x, int y)
{
    if (x >= 0 && x < terminal->width &&
        y >= 0 && y < terminal->height)
    {
        terminal->buffer[y * terminal->width + x].dirty = false;
    }
}

void terminal_set_cell(Terminal *terminal, int x, int y, TerminalCell cell)
{
    if (x >= 0 && x < terminal->width &&
        y >= 0 && y < terminal->height)
    {
        TerminalCell old_cell = terminal->buffer[y * terminal->width + x];

        if (old_cell.codepoint != cell.codepoint ||
            !terminal_attributes_equals(old_cell.attributes, cell.attributes))
        {
            terminal->buffer[y * terminal->width + x] = cell;
            terminal->buffer[y * terminal->width + x].dirty = true;

            terminal_on_paint(terminal, x, y, cell);
        }
    }
}

void terminal_cursor_show(Terminal *terminal)
{
    if (!terminal->cursor.visible)
    {
        terminal->cursor.visible = true;

        terminal_on_cursor(terminal, terminal->cursor);
    }
}

void terminal_cursor_hide(Terminal *terminal)
{
    if (terminal->cursor.visible)
    {
        terminal->cursor.visible = false;

        terminal_on_cursor(terminal, terminal->cursor);
    }
}

void terminal_cursor_move(Terminal *terminal, int offx, int offy)
{
    if (terminal->cursor.x + offx < 0)
    {
        int old_cursor_x = terminal->cursor.x;
        terminal->cursor.x = terminal->width + ((old_cursor_x + offx) % terminal->width);
        terminal_cursor_move(terminal, 0, offy + ((old_cursor_x + offx) / terminal->width - 1));
    }
    else if (terminal->cursor.x + offx >= terminal->width)
    {
        int old_cursor_x = terminal->cursor.x;
        terminal->cursor.x = (old_cursor_x + offx) % terminal->width;
        terminal_cursor_move(terminal, 0, offy + (old_cursor_x + offx) / terminal->width);
    }
    else if (terminal->cursor.y + offy < 0)
    {
        terminal_scroll(terminal, terminal->cursor.y + offy);
        terminal->cursor.y = 0;

        terminal_cursor_move(terminal, offx, 0);
    }
    else if (terminal->cursor.y + offy >= terminal->height)
    {
        terminal_scroll(terminal, (terminal->cursor.y + offy) - (terminal->height - 1));
        terminal->cursor.y = terminal->height - 1;

        terminal_cursor_move(terminal, offx, 0);
    }
    else
    {
        terminal->cursor.x += offx;
        terminal->cursor.y += offy;

        assert(terminal->cursor.x >= 0 && terminal->cursor.x < terminal->width);
        assert(terminal->cursor.y >= 0 && terminal->cursor.y < terminal->height);

        terminal_on_cursor(terminal, terminal->cursor);
    }
}

void terminal_cursor_set(Terminal *terminal, int x, int y)
{
    terminal->cursor.x = clamp(x, 0, terminal->width);
    terminal->cursor.y = clamp(y, 0, terminal->height);

    terminal_on_cursor(terminal, terminal->cursor);
}

void terminal_scroll(Terminal *terminal, int how_many_line)
{
    if (how_many_line < 0)
    {
        for (int line = 0; line < how_many_line; line++)
        {
            for (int i = (terminal->width * terminal->height) - 1; i >= terminal->height; i++)
            {
                int x = i % terminal->width;
                int y = i / terminal->width;

                terminal_set_cell(terminal, x, y, terminal_cell_at(terminal, x, y - 1));
            }

            terminal_clear_line(terminal, 0);
        }
    }
    else if (how_many_line > 0)
    {
        for (int line = 0; line < how_many_line; line++)
        {
            for (int i = 0; i < (terminal->width * (terminal->height - 1)); i++)
            {
                int x = i % terminal->width;
                int y = i / terminal->width;

                terminal_set_cell(terminal, x, y, terminal_cell_at(terminal, x, y + 1));
            }

            terminal_clear_line(terminal, terminal->height - 1);
        }
    }
}

void terminal_new_line(Terminal *terminal)
{
    terminal_cursor_move(terminal, -terminal->cursor.x, 1);
}

void terminal_backspace(Terminal *terminal)
{
    terminal_cursor_move(terminal, -1, 0);
}

void terminal_append(Terminal *terminal, Codepoint codepoint)
{
    if (codepoint == U'\n')
    {
        terminal_new_line(terminal);
    }
    else if (codepoint == U'\r')
    {
        terminal_cursor_move(terminal, -terminal->cursor.x, 0);
    }
    else if (codepoint == U'\t')
    {
        terminal_cursor_move(terminal, 8 - (terminal->cursor.x % 8), 0);
    }
    else if (codepoint == U'\b')
    {
        terminal_backspace(terminal);
    }
    else
    {
        terminal_set_cell(
            terminal,
            terminal->cursor.x,
            terminal->cursor.y,
            (TerminalCell){codepoint, terminal->current_attributes, true});

        terminal_cursor_move(terminal, 1, 0);
    }
}

void terminal_do_ansi(Terminal *terminal, Codepoint codepoint)
{
    switch (codepoint)
    {
    case U'A':
        if (terminal->parameters[0].empty)
        {
            terminal_cursor_move(terminal, 0, -1);
        }
        else
        {
            terminal_cursor_move(terminal, 0, -terminal->parameters[0].value);
        }
        break;

    case U'B':
        if (terminal->parameters[0].empty)
        {
            terminal_cursor_move(terminal, 0, 1);
        }
        else
        {
            terminal_cursor_move(terminal, 0, terminal->parameters[0].value);
        }
        break;

    case U'C':
        if (terminal->parameters[0].empty)
        {
            terminal_cursor_move(terminal, 1, 0);
        }
        else
        {
            terminal_cursor_move(terminal, terminal->parameters[0].value, 0);
        }
        break;

    case U'D':
        if (terminal->parameters[0].empty)
        {
            terminal_cursor_move(terminal, -1, 0);
        }
        else
        {
            terminal_cursor_move(terminal, -terminal->parameters[0].value, 0);
        }
        break;

    case U'E':
        if (terminal->parameters[0].empty)
        {
            terminal_cursor_move(terminal, -terminal->cursor.x, 1);
        }
        else
        {
            terminal_cursor_move(terminal, -terminal->cursor.x, terminal->parameters[0].value);
        }
        break;

    case U'F':
        if (terminal->parameters[0].empty)
        {
            terminal_cursor_move(terminal, -terminal->cursor.x, -1);
        }
        else
        {
            terminal_cursor_move(terminal, -terminal->cursor.x, -terminal->parameters[0].value);
        }
        break;

    case U'G':
        if (terminal->parameters[0].empty)
        {
            terminal_cursor_move(terminal, 0, terminal->cursor.y);
        }
        else
        {
            terminal_cursor_move(terminal, terminal->parameters[0].value - 1, terminal->cursor.y);
        }
        break;

    case U'f':
    case U'H':
    {
        int row;
        int column;

        if (terminal->parameters[0].empty)
        {
            row = 0;
        }
        else
        {
            row = terminal->parameters[0].value - 1;
        }

        if (terminal->parameters[1].empty)
        {
            column = 0;
        }
        else
        {
            column = terminal->parameters[1].value - 1;
        }

        terminal_cursor_set(terminal, column, row);
    }
    break;

    case U'J':
        if (terminal->parameters[0].value == 0)
        {
            terminal_clear(terminal, terminal->cursor.x, terminal->cursor.y, terminal->width, terminal->height);
        }
        else if (terminal->parameters[0].value == 1)
        {
            terminal_clear(terminal, 0, 0, terminal->cursor.x, terminal->cursor.y);
        }
        else if (terminal->parameters[0].value == 2)
        {
            terminal_clear(terminal, 0, 0, terminal->width, terminal->height);
        }
        break;

    case U'K':
        if (terminal->parameters[0].value == 0)
        {
            terminal_clear(terminal, terminal->cursor.x, terminal->cursor.y, terminal->width, terminal->cursor.y);
        }
        else if (terminal->parameters[0].value == 1)
        {
            terminal_clear(terminal, 0, terminal->cursor.y, terminal->cursor.x, terminal->cursor.y);
        }
        else if (terminal->parameters[0].value == 2)
        {
            terminal_clear(terminal, 0, terminal->cursor.y, terminal->width, terminal->cursor.y);
        }
        break;

    case U'S':
        if (terminal->parameters[0].empty)
        {
            terminal_scroll(terminal, -1);
        }
        else
        {
            terminal_scroll(terminal, -terminal->parameters[0].value);
        }
        break;

    case U'T':
        if (terminal->parameters[0].empty)
        {
            terminal_scroll(terminal, 1);
        }
        else
        {
            terminal_scroll(terminal, terminal->parameters[0].value);
        }
        break;

    case U'm':
        for (int i = 0; i <= terminal->parameters_top; i++)
        {
            if (terminal->parameters[i].empty || terminal->parameters[i].value == 0)
            {
                terminal->current_attributes = terminal->default_attributes;
            }
            else
            {
                int attr = terminal->parameters[i].value;

                if (attr == 1)
                {
                    terminal->current_attributes = terminal_attributes_bold(terminal->current_attributes);
                }
                else if (attr == 3)
                {
                    terminal->current_attributes = terminal_attributes_inverted(terminal->current_attributes);
                }
                else if (attr == 4)
                {
                    terminal->current_attributes = terminal_attributes_underline(terminal->current_attributes);
                }
                else if (attr >= 30 && attr <= 37)
                {
                    terminal->current_attributes = terminal_attributes_with_foreground(terminal->current_attributes, attr - 30);
                }
                else if (attr >= 90 && attr <= 97)
                {
                    terminal->current_attributes = terminal_attributes_with_foreground(terminal->current_attributes, attr - 90 + 8);
                }
                else if (attr >= 40 && attr <= 47)
                {
                    terminal->current_attributes = terminal_attributes_with_background(terminal->current_attributes, attr - 40);
                }
                else if (attr >= 100 && attr <= 107)
                {
                    terminal->current_attributes = terminal_attributes_with_background(terminal->current_attributes, attr - 100 + 8);
                }
            }
        }
        break;

    case U's':
        terminal->saved_cursor = terminal->cursor;
        break;

    case U'u':
        terminal->cursor = terminal->saved_cursor;
        break;

    default:
        break;
    }
}

void terminal_write_codepoint(Terminal *terminal, Codepoint codepoint)
{
    switch (terminal->state)
    {
    case TERMINAL_STATE_WAIT_ESC:
        if (codepoint == U'\e')
        {
            for (int i = 0; i < TERMINAL_MAX_PARAMETERS; i++)
            {
                terminal->parameters[i].empty = true;
                terminal->parameters[i].value = 0;
            }

            terminal->parameters_top = 0;

            terminal->state = TERMINAL_STATE_EXPECT_BRACKET;
        }
        else
        {
            terminal->state = TERMINAL_STATE_WAIT_ESC;
            terminal_append(terminal, codepoint);
        }
        break;

    case TERMINAL_STATE_EXPECT_BRACKET:
        if (codepoint == U'[')
        {
            terminal->state = TERMINAL_STATE_READ_ATTRIBUTE;
        }
        else if (codepoint == U'c')
        {
            terminal->current_attributes = terminal->default_attributes;
            terminal->state = TERMINAL_STATE_WAIT_ESC;

            terminal_cursor_set(terminal, 0, 0);
            terminal_clear_all(terminal);
        }
        else
        {
            terminal->state = TERMINAL_STATE_WAIT_ESC;
            terminal_append(terminal, codepoint);
        }
        break;

    case TERMINAL_STATE_READ_ATTRIBUTE:
        if (codepoint_is_digit(codepoint))
        {
            terminal->parameters[terminal->parameters_top].empty = false;
            terminal->parameters[terminal->parameters_top].value *= 10;
            terminal->parameters[terminal->parameters_top].value += codepoint_numeric_value(codepoint);
        }
        else
        {
            if (codepoint == U';')
            {
                terminal->parameters_top++;
            }
            else
            {
                terminal->state = TERMINAL_STATE_WAIT_ESC;

                terminal_do_ansi(terminal, codepoint);
            }
        }
        break;

    default:
        ASSERT_NOT_REACHED();
        break;
    }
}

void terminal_write_char(Terminal *terminal, char c)
{
    utf8decoder_write(terminal->decoder, c);
}

void terminal_write(Terminal *terminal, const char *buffer, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        terminal_write_char(terminal, buffer[i]);
    }
}

void terminal_on_paint(Terminal *terminal, int x, int y, TerminalCell cell)
{
    if (terminal->renderer->on_paint)
    {
        terminal->renderer->on_paint(terminal, terminal->renderer, x, y, cell);
    }
}

void terminal_on_cursor(Terminal *terminal, TerminalCursor cursor)
{
    if (terminal->renderer->on_cursor)
    {
        terminal->renderer->on_cursor(terminal, terminal->renderer, cursor);
    }
}

void terminal_on_blink(Terminal *terminal)
{
    if (terminal->renderer->on_blink)
    {
        terminal->renderer->on_blink(terminal, terminal->renderer);
    }
}

void terminal_blink(Terminal *terminal)
{
    terminal_on_blink(terminal);
}

void terminal_repaint(Terminal *terminal)
{
    if (terminal->renderer->repaint)
    {
        terminal->renderer->repaint(terminal, terminal->renderer);
    }
}
