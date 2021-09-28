#pragma once

#include <libio/Write.h>
#include <libio/Writer.h>

namespace IO
{

struct Prettier :
    public IO::Writer
{
private:
    IO::Writer &_writer;
    int _depth = 0;
    int _flags;

public:
    static constexpr auto NONE = 0;
    static constexpr auto COLORS = 1 << 0;
    static constexpr auto INDENTS = 1 << 1;
    static constexpr auto USETAB = 1 << 2;

    Prettier(IO::Writer &writer, int flags = NONE)
        : _writer{writer},
          _flags{flags}
    {
    }

    ResultOr<size_t> write(const void *buffer, size_t size) override
    {
        return _writer.write(buffer, size);
    }

    HjResult flush() override
    {
        return _writer.flush();
    }

    void ident()
    {
        if (_flags & INDENTS)
        {
            IO::write(_writer, '\n');

            for (int i = 0; i < _depth; i++)
            {
                if (_flags & USETAB)
                {
                    IO::write(_writer, "\t");
                }
                else
                {
                    IO::write(_writer, "    ");
                }
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

            IO::write(_writer, depth_color[_depth % 6]);
        }
    }

    void color_clear()
    {
        if (_flags & COLORS)
        {
            IO::write(_writer, "\e[m");
        }
    }
};

} // namespace IO
