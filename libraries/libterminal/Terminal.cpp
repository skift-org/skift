
#include <libsystem/Assert.h>
#include <libsystem/Logger.h>
#include <libsystem/math/MinMax.h>
#include <libterminal/Terminal.h>

namespace terminal
{

Terminal::Terminal(int width, int height)
{
    _width = width;
    _height = height;
    _buffer = (Cell *)calloc(_width * _height, sizeof(Cell));

    _decoder.callback([this](auto codepoint) { write(codepoint); });

    _cursor = {0, 0, true};
    _saved_cursor = {0, 0, true};

    _attributes = Attributes::defaults();

    _parameters_top = 0;

    for (auto &_parameter : _parameters)
    {
        _parameter.empty = true;
        _parameter.value = 0;
    }

    clear_all();
}

Terminal::~Terminal()
{
    free(_buffer);
}

void Terminal::clear(int fromx, int fromy, int tox, int toy)
{
    for (int i = fromx + fromy * _width; i < tox + toy * _width; i++)
    {
        set_cell(i % _width, i / _width, (Cell){U' ', _attributes, true});
    }
}

void Terminal::clear_all()
{
    Terminal::clear(0, 0, _width, _height);
}

void Terminal::clear_line(int line)
{
    if (line >= 0 && line < _height)
    {
        for (int i = 0; i < _width; i++)
        {
            set_cell(i, line, (Cell){U' ', _attributes, true});
        }
    }
}

void Terminal::resize(int width, int height)
{
    Cell *new_buffer = (Cell *)malloc(sizeof(Cell) * width * height);

    for (int i = 0; i < width * height; i++)
    {
        new_buffer[i] = {U' ', _attributes, true};
    }

    for (int x = 0; x < MIN(width, _width); x++)
    {
        for (int y = 0; y < MIN(height, _height); y++)
        {
            new_buffer[y * width + x] = cell_at(x, y);
        }
    }

    free(_buffer);
    _buffer = new_buffer;

    _width = width;
    _height = height;

    _cursor.x = clamp(_cursor.x, 0, width - 1);
    _cursor.y = clamp(_cursor.y, 0, height - 1);
}

Cell Terminal::cell_at(int x, int y)
{
    if (x >= 0 && x < _width && y >= 0 && y < _height)
    {
        return _buffer[y * _width + x];
    }

    return {U' ', _attributes, true};
}

void Terminal::cell_undirty(int x, int y)
{
    if (x >= 0 && x < _width && y >= 0 && y < _height)
    {
        _buffer[y * _width + x].dirty = false;
    }
}

void Terminal::set_cell(int x, int y, Cell cell)
{
    if (x >= 0 && x < _width &&
        y >= 0 && y < _height)
    {
        Cell old_cell = _buffer[y * _width + x];

        if (old_cell.codepoint != cell.codepoint ||
            old_cell.attributes != cell.attributes)
        {
            _buffer[y * _width + x] = cell;
            _buffer[y * _width + x].dirty = true;
        }
    }
}

void Terminal::cursor_move(int offx, int offy)
{
    if (_cursor.x + offx < 0)
    {
        int old_cursor_x = _cursor.x;
        _cursor.x = _width + ((old_cursor_x + offx) % _width);
        cursor_move(0, offy + ((old_cursor_x + offx) / _width - 1));
    }
    else if (_cursor.x + offx >= _width)
    {
        int old_cursor_x = _cursor.x;
        _cursor.x = (old_cursor_x + offx) % _width;
        cursor_move(0, offy + (old_cursor_x + offx) / _width);
    }
    else if (_cursor.y + offy < 0)
    {
        scroll(_cursor.y + offy);
        _cursor.y = 0;

        cursor_move(offx, 0);
    }
    else if (_cursor.y + offy >= _height)
    {
        scroll((_cursor.y + offy) - (_height - 1));
        _cursor.y = _height - 1;

        cursor_move(offx, 0);
    }
    else
    {
        _cursor.x += offx;
        _cursor.y += offy;

        assert(_cursor.x >= 0 && _cursor.x < _width);
        assert(_cursor.y >= 0 && _cursor.y < _height);
    }
}

void Terminal::cursor_set(int x, int y)
{
    _cursor.x = clamp(x, 0, _width);
    _cursor.y = clamp(y, 0, _height);
}

void Terminal::scroll(int how_many_line)
{
    if (how_many_line < 0)
    {
        for (int line = 0; line < how_many_line; line++)
        {
            for (int i = (_width * _height) - 1; i >= _height; i++)
            {
                int x = i % _width;
                int y = i / _width;

                set_cell(x, y, cell_at(x, y - 1));
            }

            clear_line(0);
        }
    }
    else if (how_many_line > 0)
    {
        for (int line = 0; line < how_many_line; line++)
        {
            for (int i = 0; i < _width * (_height - 1); i++)
            {
                int x = i % _width;
                int y = i / _width;

                set_cell(x, y, cell_at(x, y + 1));
            }

            clear_line(_height - 1);
        }
    }
}

void Terminal::new_line()
{
    cursor_move(-_cursor.x, 1);
}

void Terminal::backspace()
{
    cursor_move(-1, 0);
}

void Terminal::append(Codepoint codepoint)
{
    if (codepoint == U'\n')
    {
        new_line();
    }
    else if (codepoint == U'\r')
    {
        cursor_move(-_cursor.x, 0);
    }
    else if (codepoint == U'\t')
    {
        cursor_move(8 - (_cursor.x % 8), 0);
    }
    else if (codepoint == U'\b')
    {
        backspace();
    }
    else
    {
        set_cell(_cursor.x, _cursor.y, {codepoint, _attributes, true});
        cursor_move(1, 0);
    }
}

void Terminal::do_ansi(Codepoint codepoint)
{
    switch (codepoint)
    {
    case U'A':
        if (_parameters[0].empty)
        {
            cursor_move(0, -1);
        }
        else
        {
            cursor_move(0, -_parameters[0].value);
        }
        break;

    case U'B':
        if (_parameters[0].empty)
        {
            cursor_move(0, 1);
        }
        else
        {
            cursor_move(0, _parameters[0].value);
        }
        break;

    case U'C':
        if (_parameters[0].empty)
        {
            cursor_move(1, 0);
        }
        else
        {
            cursor_move(_parameters[0].value, 0);
        }
        break;

    case U'D':
        if (_parameters[0].empty)
        {
            cursor_move(-1, 0);
        }
        else
        {
            cursor_move(-_parameters[0].value, 0);
        }
        break;

    case U'E':
        if (_parameters[0].empty)
        {
            cursor_move(-_cursor.x, 1);
        }
        else
        {
            cursor_move(-_cursor.x, _parameters[0].value);
        }
        break;

    case U'F':
        if (_parameters[0].empty)
        {
            cursor_move(-_cursor.x, -1);
        }
        else
        {
            cursor_move(-_cursor.x, -_parameters[0].value);
        }
        break;

    case U'G':
        if (_parameters[0].empty)
        {
            cursor_move(0, _cursor.y);
        }
        else
        {
            cursor_move(_parameters[0].value - 1, _cursor.y);
        }
        break;

    case U'f':
    case U'H':
    {
        int row;
        int column;

        if (_parameters[0].empty)
        {
            row = 0;
        }
        else
        {
            row = _parameters[0].value - 1;
        }

        if (_parameters[1].empty)
        {
            column = 0;
        }
        else
        {
            column = _parameters[1].value - 1;
        }

        cursor_set(column, row);
    }
    break;

    case U'J':
        if (_parameters[0].value == 0)
        {
            clear(_cursor.x, _cursor.y, _width, _height);
        }
        else if (_parameters[0].value == 1)
        {
            clear(0, 0, _cursor.x, _cursor.y);
        }
        else if (_parameters[0].value == 2)
        {
            clear(0, 0, _width, _height);
        }
        break;

    case U'K':
        if (_parameters[0].value == 0)
        {
            clear(_cursor.x, _cursor.y, _width, _cursor.y);
        }
        else if (_parameters[0].value == 1)
        {
            clear(0, _cursor.y, _cursor.x, _cursor.y);
        }
        else if (_parameters[0].value == 2)
        {
            clear(0, _cursor.y, _width, _cursor.y);
        }
        break;

    case U'S':
        if (_parameters[0].empty)
        {
            scroll(-1);
        }
        else
        {
            scroll(-_parameters[0].value);
        }
        break;

    case U'T':
        if (_parameters[0].empty)
        {
            scroll(1);
        }
        else
        {
            scroll(_parameters[0].value);
        }
        break;

    case U'm':
        for (int i = 0; i <= _parameters_top; i++)
        {
            if (_parameters[i].empty || _parameters[i].value == 0)
            {
                _attributes = Attributes::defaults();
            }
            else
            {
                int attr = _parameters[i].value;

                if (attr == 1)
                {
                    _attributes = _attributes.bolded();
                }
                else if (attr == 3)
                {
                    _attributes = _attributes.inverted();
                }
                else if (attr == 4)
                {
                    _attributes = _attributes.underlined();
                }
                else if (attr >= 30 && attr <= 37)
                {
                    _attributes = _attributes.with_forground((Color)(attr - 30));
                }
                else if (attr >= 90 && attr <= 97)
                {
                    _attributes = _attributes.with_forground((Color)(attr - 90 + 8));
                }
                else if (attr >= 40 && attr <= 47)
                {
                    _attributes = _attributes.with_background((Color)(attr - 40));
                }
                else if (attr >= 100 && attr <= 107)
                {
                    _attributes = _attributes.with_background((Color)(attr - 100 + 8));
                }
            }
        }
        break;

    case U's':
        _saved_cursor = _cursor;
        break;

    case U'u':
        _cursor = _saved_cursor;
        break;

    default:
        break;
    }
}

void Terminal::write(Codepoint codepoint)
{
    switch (_state)
    {
    case State::WAIT_ESC:
        if (codepoint == U'\e')
        {
            for (auto &_parameter : _parameters)
            {
                _parameter.empty = true;
                _parameter.value = 0;
            }

            _parameters_top = 0;

            _state = State::EXPECT_BRACKET;
        }
        else
        {
            _state = State::WAIT_ESC;
            append(codepoint);
        }
        break;

    case State::EXPECT_BRACKET:
        if (codepoint == U'[')
        {
            _state = State::READ_ATTRIBUTE;
        }
        else if (codepoint == U'c')
        {
            _attributes = Attributes::defaults();
            _state = State::WAIT_ESC;

            cursor_set(0, 0);
            clear_all();
        }
        else
        {
            _state = State::WAIT_ESC;
            append(codepoint);
        }
        break;

    case State::READ_ATTRIBUTE:
        if (codepoint_is_digit(codepoint))
        {
            _parameters[_parameters_top].empty = false;
            _parameters[_parameters_top].value *= 10;
            _parameters[_parameters_top].value += codepoint_numeric_value(codepoint);
        }
        else
        {
            if (codepoint == U';')
            {
                _parameters_top++;
            }
            else
            {
                _state = State::WAIT_ESC;

                do_ansi(codepoint);
            }
        }
        break;

    default:
        logger_trace("ok wtf is going on here ? %d", _state);
        ASSERT_NOT_REACHED();
    }
}

void Terminal::write(char c)
{
    _decoder.write(c);
}

void Terminal::write(const char *buffer, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        write(buffer[i]);
    }
}

} // namespace terminal
