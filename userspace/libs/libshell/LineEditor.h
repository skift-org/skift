#pragma once

#include <libshell/Line.h>

namespace Shell
{

struct LineEditor
{
private:
    Line _line;
    size_t _cursor = 0;

public:
    const Line &line() const { return _line; }

    size_t cursor() { return _cursor; }

    void line(const Line &line)
    {
        _line = line;
        _cursor = line.count();
    }

    void move_left()
    {
        if (_cursor > 0)
        {
            _cursor--;
        }
    }

    void move_right()
    {
        if (_cursor < _line.count())
        {
            _cursor++;
        }
    }

    void move_home()
    {
        _cursor = 0;
    }

    void move_end()
    {
        _cursor = _line.count();
    }

    void insert(Text::Rune cp)
    {
        _line.insert(_cursor, cp);
        _cursor++;
    }

    void delete_()
    {
        if (_cursor < _line.count())
        {
            _line.remove_index(_cursor);
        }
    }

    void backspace()
    {
        if (_cursor > 0)
        {
            _cursor--;
            delete_();
        }
    }
};

} // namespace Shell
