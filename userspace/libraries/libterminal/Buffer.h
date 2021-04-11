#pragma once

#include <libterminal/Cell.h>
#include <libutils/Assert.h>
#include <libutils/Vector.h>

namespace Terminal
{

class Buffer
{
private:
    int _width;
    int _height;
    Vector<Cell> _buffer;

public:
    int width() const { return _width; }

    int height() const { return _height; }

    Buffer(int width, int height)
        : _width{width}, _height{height}
    {
        _buffer.resize(_width * _height);
    }

    const Cell at(int x, int y) const
    {
        if (x >= 0 && x < _width && y >= 0 && y < _height)
        {
            return _buffer[y * _width + x];
        }

        return {U' ', {}, true};
    }

    void set(int x, int y, Cell cell)
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

    void undirty(int x, int y)
    {
        if (x >= 0 && x < _width && y >= 0 && y < _height)
        {
            _buffer[y * _width + x].dirty = false;
        }
    }

    void clear(int fromx, int fromy, int tox, int toy, Attributes attributes)
    {
        for (int i = fromx + fromy * _width; i < tox + toy * _width; i++)
        {
            set(i % _width, i / _width, (Cell){U' ', attributes, true});
        }
    }

    void clear_all(Attributes attributes)
    {
        clear(0, 0, _width, _height, attributes);
    }

    void clear_line(int line, Attributes attributes)
    {
        if (line >= 0 && line < _height)
        {
            for (int i = 0; i < _width; i++)
            {
                set(i, line, (Cell){U' ', attributes, true});
            }
        }
    }

    void resize(int width, int height)
    {
        Vector<Cell> new_buffer;
        new_buffer.resize(width * height);

        for (int i = 0; i < width * height; i++)
        {
            new_buffer[i] = {U' ', {}, true};
        }

        for (int x = 0; x < MIN(width, _width); x++)
        {
            for (int y = 0; y < MIN(height, _height); y++)
            {
                new_buffer[y * width + x] = at(x, y);
            }
        }

        _buffer = new_buffer;

        _width = width;
        _height = height;
    }

    void scroll(int how_many_line, Attributes attributes)
    {
        if (how_many_line < 0)
        {
            for (int line = 0; line < how_many_line; line++)
            {
                for (int i = (width() * height()) - 1; i >= height(); i++)
                {
                    int x = i % width();
                    int y = i / width();

                    set(x, y, at(x, y - 1));
                }

                clear_line(0, attributes);
            }
        }
        else if (how_many_line > 0)
        {
            for (int line = 0; line < how_many_line; line++)
            {
                for (int i = 0; i < width() * (height() - 1); i++)
                {
                    int x = i % width();
                    int y = i / width();

                    set(x, y, at(x, y + 1));
                }

                clear_line(height() - 1, attributes);
            }
        }
    }
};

} // namespace Terminal
