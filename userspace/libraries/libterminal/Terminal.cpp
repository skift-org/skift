#include <assert.h>

#include <libmath/MinMax.h>
#include <libsystem/Logger.h>
#include <libterminal/Terminal.h>

namespace Terminal
{

Terminal::Terminal(int width, int height)
    : _surface{width, height}
{
    _decoder.callback([this](auto codepoint) { write(codepoint); });

    _cursor = {0, 0, true};
    _saved_cursor = {0, 0, true};

    _parameters_top = 0;

    for (auto &_parameter : _parameters)
    {
        _parameter.empty = true;
        _parameter.value = 0;
    }
}

void Terminal::resize(int width, int height)
{
    _surface.resize(width, height);

    _cursor.x = clamp(_cursor.x, 0, width - 1);
    _cursor.y = clamp(_cursor.y, 0, height - 1);
}

void Terminal::cursor_move(int offx, int offy)
{
    if (_cursor.x + offx < 0)
    {
        int old_cursor_x = _cursor.x;
        _cursor.x = width() + ((old_cursor_x + offx) % width());
        cursor_move(0, offy + ((old_cursor_x + offx) / width() - 1));
    }
    else if (_cursor.x + offx >= width())
    {
        int old_cursor_x = _cursor.x;
        _cursor.x = (old_cursor_x + offx) % width();
        cursor_move(0, offy + (old_cursor_x + offx) / width());
    }
    else if (_cursor.y + offy < 0)
    {
        _surface.scroll(_cursor.y + offy, _attributes);
        _cursor.y = 0;

        cursor_move(offx, 0);
    }
    else if (_cursor.y + offy >= height())
    {
        _surface.scroll((_cursor.y + offy) - (height() - 1), _attributes);
        _cursor.y = height() - 1;

        cursor_move(offx, 0);
    }
    else
    {
        _cursor.x += offx;
        _cursor.y += offy;

        assert(_cursor.x >= 0 && _cursor.x < width());
        assert(_cursor.y >= 0 && _cursor.y < height());
    }
}

void Terminal::cursor_set(int x, int y)
{
    _cursor.x = clamp(x, 0, width());
    _cursor.y = clamp(y, 0, height());
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
        _surface.set(_cursor.x, _cursor.y, {codepoint, _attributes, true});
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
            _surface.clear(_cursor.x, _cursor.y, width(), height(), _attributes);
        }
        else if (_parameters[0].value == 1)
        {
            _surface.clear(0, 0, _cursor.x, _cursor.y, _attributes);
        }
        else if (_parameters[0].value == 2)
        {
            _surface.clear(0, 0, width(), height(), _attributes);
        }
        break;

    case U'K':
        if (_parameters[0].value == 0)
        {
            _surface.clear(_cursor.x, _cursor.y, width(), _cursor.y, _attributes);
        }
        else if (_parameters[0].value == 1)
        {
            _surface.clear(0, _cursor.y, _cursor.x, _cursor.y, _attributes);
        }
        else if (_parameters[0].value == 2)
        {
            _surface.clear(0, _cursor.y, width(), _cursor.y, _attributes);
        }
        break;

    case U'S':
        if (_parameters[0].empty)
        {
            _surface.scroll(-1, _attributes);
        }
        else
        {
            _surface.scroll(-_parameters[0].value, _attributes);
        }
        break;

    case U'T':
        if (_parameters[0].empty)
        {
            _surface.scroll(1, _attributes);
        }
        else
        {
            _surface.scroll(_parameters[0].value, _attributes);
        }
        break;

    case U'm':
        for (int i = 0; i <= _parameters_top; i++)
        {
            if (_parameters[i].empty || _parameters[i].value == 0)
            {
                _attributes = {};
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
            _attributes = {};
            _state = State::WAIT_ESC;

            cursor_set(0, 0);
            _surface.clear_all(_attributes);
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

} // namespace Terminal
