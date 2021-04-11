#pragma once

#include <libutils/StringBuilder.h>

struct Prettifier : public StringBuilder
{
private:
    int _depth = 0;
    int _flags;

public:
    static constexpr auto NONE = 0;
    static constexpr auto COLORS = 1 << 0;
    static constexpr auto INDENTS = 1 << 1;

    Prettifier(int flags = NONE) : _flags(flags)
    {
    }

    void ident()
    {
        if (_flags & INDENTS)
        {
            append('\n');

            for (int i = 0; i < _depth; i++)
            {
                append("    ");
            }
        }
    }

    void push_ident()
    {
        _depth++;
    }

    void pop_ident()
    {
        assert(_depth);
        _depth--;
    }

    void color_depth()
    {
        if (_flags & COLORS)
        {
            const char *depth_color[] = {
                "\e[91m",
                "\e[92m",
                "\e[93m",
                "\e[94m",
                "\e[95m",
                "\e[96m",
            };

            append(depth_color[_depth % 6]);
        }
    }

    void color_clear()
    {
        if (_flags & COLORS)
        {
            append("\e[m");
        }
    }
};
