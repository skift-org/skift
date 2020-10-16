
#include <libsystem/Assert.h>
#include <libsystem/math/MinMax.h>
#include <libterminal/Terminal.h>

Terminal::Terminal(int width, int height)
{
    this->width = width;
    this->height = height;
    buffer = (TerminalCell *)calloc(width * height, sizeof(TerminalCell));

    decoder = utf8decoder_create(this, (UTF8DecoderCallback)terminal_write_codepoint);

    cursor = (TerminalCursor){0, 0, true};
    saved_cursor = (TerminalCursor){0, 0, true};

    current_attributes = (TerminalAttributes){TERMINAL_COLOR_DEFAULT_FOREGROUND, TERMINAL_COLOR_DEFAULT_BACKGROUND, false, false, false};
    default_attributes = (TerminalAttributes){TERMINAL_COLOR_DEFAULT_FOREGROUND, TERMINAL_COLOR_DEFAULT_BACKGROUND, false, false, false};

    parameters_top = 0;

    for (int i = 0; i < TERMINAL_MAX_PARAMETERS; i++)
    {
        parameters[i].empty = true;
        parameters[i].value = 0;
    }

    clear_all();
}

Terminal::~Terminal()
{
    utf8decoder_destroy(decoder);
    free(buffer);
}

void Terminal::clear(int fromx, int fromy, int tox, int toy)
{
    for (int i = fromx + fromy * width; i < tox + toy * width; i++)
    {
        set_cell(
            i % width,
            i / width,
            (TerminalCell){U' ', current_attributes, true});
    }
}

void Terminal::clear_all()
{
    Terminal::clear(0, 0, width, height);
}

void Terminal::clear_line(int line)
{
    if (line >= 0 && line < height)
    {
        for (int i = 0; i < width; i++)
        {
            set_cell(
                i,
                line,
                (TerminalCell){U' ', current_attributes, true});
        }
    }
}

void Terminal::resize(int width, int height)
{
    TerminalCell *new_buffer = (TerminalCell *)malloc(sizeof(TerminalCell) * width * height);

    for (int i = 0; i < width * height; i++)
    {
        new_buffer[i] = (TerminalCell){U' ', current_attributes, true};
    }

    for (int x = 0; x < MIN(width, this->width); x++)
    {
        for (int y = 0; y < MIN(height, this->height); y++)
        {
            new_buffer[y * width + x] = cell_at(x, y);
        }
    }

    free(buffer);
    buffer = new_buffer;

    this->width = width;
    this->height = height;

    cursor.x = clamp(cursor.x, 0, width);
    cursor.y = clamp(cursor.y, 0, height);
}

TerminalCell Terminal::cell_at(int x, int y)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        return buffer[y * width + x];
    }

    return (TerminalCell){U' ', current_attributes, true};
}

void Terminal::cell_undirty(int x, int y)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        buffer[y * width + x].dirty = false;
    }
}

void Terminal::set_cell(int x, int y, TerminalCell cell)
{
    if (x >= 0 && x < width &&
        y >= 0 && y < height)
    {
        TerminalCell old_cell = buffer[y * width + x];

        if (old_cell.codepoint != cell.codepoint ||
            old_cell.attributes != cell.attributes)
        {
            buffer[y * width + x] = cell;
            buffer[y * width + x].dirty = true;
        }
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
    }
}

void terminal_cursor_set(Terminal *terminal, int x, int y)
{
    terminal->cursor.x = clamp(x, 0, terminal->width);
    terminal->cursor.y = clamp(y, 0, terminal->height);
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

                terminal->set_cell(x, y, terminal->cell_at(x, y - 1));
            }

            terminal->clear_line(0);
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

                terminal->set_cell(x, y, terminal->cell_at(x, y + 1));
            }

            terminal->clear_line(terminal->height - 1);
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
        terminal->set_cell(
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
            terminal->clear(terminal->cursor.x, terminal->cursor.y, terminal->width, terminal->height);
        }
        else if (terminal->parameters[0].value == 1)
        {
            terminal->clear(0, 0, terminal->cursor.x, terminal->cursor.y);
        }
        else if (terminal->parameters[0].value == 2)
        {
            terminal->clear(0, 0, terminal->width, terminal->height);
        }
        break;

    case U'K':
        if (terminal->parameters[0].value == 0)
        {
            terminal->clear(terminal->cursor.x, terminal->cursor.y, terminal->width, terminal->cursor.y);
        }
        else if (terminal->parameters[0].value == 1)
        {
            terminal->clear(0, terminal->cursor.y, terminal->cursor.x, terminal->cursor.y);
        }
        else if (terminal->parameters[0].value == 2)
        {
            terminal->clear(0, terminal->cursor.y, terminal->width, terminal->cursor.y);
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
                    terminal->current_attributes = terminal->current_attributes.bolded();
                }
                else if (attr == 3)
                {
                    terminal->current_attributes = terminal->current_attributes.inverted();
                }
                else if (attr == 4)
                {
                    terminal->current_attributes = terminal->current_attributes.underlined();
                }
                else if (attr >= 30 && attr <= 37)
                {
                    terminal->current_attributes = terminal->current_attributes.with_forground((TerminalColor)(attr - 30));
                }
                else if (attr >= 90 && attr <= 97)
                {
                    terminal->current_attributes = terminal->current_attributes.with_forground((TerminalColor)(attr - 90 + 8));
                }
                else if (attr >= 40 && attr <= 47)
                {
                    terminal->current_attributes = terminal->current_attributes.with_background((TerminalColor)(attr - 40));
                }
                else if (attr >= 100 && attr <= 107)
                {
                    terminal->current_attributes = terminal->current_attributes.with_background((TerminalColor)(attr - 100 + 8));
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
    case TerminalState::WAIT_ESC:
        if (codepoint == U'\e')
        {
            for (int i = 0; i < TERMINAL_MAX_PARAMETERS; i++)
            {
                terminal->parameters[i].empty = true;
                terminal->parameters[i].value = 0;
            }

            terminal->parameters_top = 0;

            terminal->state = TerminalState::EXPECT_BRACKET;
        }
        else
        {
            terminal->state = TerminalState::WAIT_ESC;
            terminal_append(terminal, codepoint);
        }
        break;

    case TerminalState::EXPECT_BRACKET:
        if (codepoint == U'[')
        {
            terminal->state = TerminalState::READ_ATTRIBUTE;
        }
        else if (codepoint == U'c')
        {
            terminal->current_attributes = terminal->default_attributes;
            terminal->state = TerminalState::WAIT_ESC;

            terminal_cursor_set(terminal, 0, 0);
            terminal->clear_all();
        }
        else
        {
            terminal->state = TerminalState::WAIT_ESC;
            terminal_append(terminal, codepoint);
        }
        break;

    case TerminalState::READ_ATTRIBUTE:
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
                terminal->state = TerminalState::WAIT_ESC;

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
