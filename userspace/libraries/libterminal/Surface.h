#pragma once

#include <libterminal/Buffer.h>

namespace Terminal
{

struct Surface
{
private:
    Buffer _buffer;

    int _height;
    int _width;

    int _scrollback = 0;

    int convert_y(int y) const
    {
        return _buffer.height() - _height + y;
    }

public:
    int width() { return _width; }

    int height() { return _height; }

    int scrollback() { return _scrollback; }

    Surface(int width, int height)
        : _buffer{width, 1000}
    {
        _width = width;
        _height = height;
    }

    const Cell at(int x, int y) const
    {
        return _buffer.at(x, convert_y(y));
    }

    void set(int x, int y, Cell cell)
    {
        _buffer.set(x, convert_y(y), cell);
    }

    void undirty(int x, int y)
    {
        _buffer.undirty(x, convert_y(y));
    }

    void clear(int fromx, int fromy, int tox, int toy, Attributes attributes)
    {
        _buffer.clear(fromx, convert_y(fromy), tox, convert_y(toy), attributes);
    }

    void clear_all(Attributes attributes)
    {
        clear(0, 0, _width, _height, attributes);
    }

    void clear_line(int line, Attributes attributes)
    {
        _buffer.clear_line(convert_y(line), attributes);
    }

    void resize(int width, int height)
    {
        _buffer.resize(MAX(_buffer.width(), width), MAX(MAX(height, _height), 1000));
        _width = width;
        _height = height;
    }

    void scroll(int how_many_line, Attributes attributes)
    {
        _buffer.scroll(how_many_line, attributes);
        _scrollback += how_many_line;
    }
};

} // namespace Terminal
