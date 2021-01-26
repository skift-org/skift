#pragma once

#include <libsystem/Assert.h>
#include <libsystem/core/CString.h>

#include <libutils/Move.h>

class RingBuffer
{
private:
    size_t _head = 0;
    size_t _tail = 0;
    size_t _size = 0;
    size_t _used = 0;

    char *_buffer = nullptr;

public:
    RingBuffer(size_t size)
    {
        _size = size;
        _buffer = new char[size];
    }

    RingBuffer(const RingBuffer &other) : _head(other._head),
                                          _tail(other._tail),
                                          _size(other._size),
                                          _used(other._used)
    {
        _buffer = new char[other._size];
        memcpy(_buffer, other._buffer, other._size);
    }

    RingBuffer(RingBuffer &&other)
        : _head(other._head),
          _tail(other._tail),
          _size(other._size),
          _used(other._used),
          _buffer(other._buffer)
    {
        other._head = 0;
        other._tail = 0;
        other._size = 0;
        other._used = 0;
        other._buffer = nullptr;
    }

    RingBuffer &operator=(const RingBuffer &other)
    {
        return *this = RingBuffer(other);
    }

    RingBuffer &operator=(RingBuffer &&other)
    {
        swap(_head, other._head);
        swap(_tail, other._tail);
        swap(_size, other._size);
        swap(_used, other._used);
        swap(_buffer, other._buffer);

        return *this;
    }

    ~RingBuffer()
    {
        if (_buffer)
            delete[] _buffer;
    }

    bool empty() const
    {
        return _used == 0;
    }

    bool full() const
    {
        return _used == _size;
    }

    size_t used() const
    {
        return _used;
    }

    void put(char c)
    {
        assert(!full());

        _buffer[_head] = c;
        _head = (_head + 1) % (_size);
        _used++;
    }

    char get()
    {
        assert(!empty());

        char c = _buffer[_tail];
        _tail = (_tail + 1) % (_size);
        _used--;

        return c;
    }

    char peek(size_t peek)
    {
        int offset = (_tail + peek) % (_size);

        return _buffer[offset];
    }

    size_t read(char *buffer, size_t size)
    {
        size_t read = 0;

        while (!empty() && read < size)
        {
            buffer[read] = get();
            read++;
        }

        return read;
    }

    size_t write(const char *buffer, size_t size)
    {
        size_t written = 0;

        while (!full() && written < size)
        {
            put(buffer[written]);
            written++;
        }

        return written;
    }
};
