#pragma once

#include <libsystem/Assert.h>
#include <libsystem/Common.h>
#include <libsystem/math/MinMax.h>
#include <libutils/String.h>
#include <libutils/Vector.h>

struct StringBuilder
{
    __noncopyable(StringBuilder);
    __nonmovable(StringBuilder);

private:
    size_t _used = 0;
    size_t _size = 0;
    char *_buffer = nullptr;

public:
    size_t length() const
    {
        return _used;
    }

    StringBuilder() : StringBuilder(16) {}

    StringBuilder(size_t preallocated)
    {
        preallocated = MAX(preallocated, 16);

        _buffer = new char[preallocated];
        _size = preallocated;
        _used = 0;
    }

    ~StringBuilder()
    {
        if (_buffer)
            delete[] _buffer;
    }

    String finalize()
    {
        char *result = _buffer;
        size_t size = _used;

        _buffer = nullptr;
        _used = 0;
        _size = 0;

        return String(make<StringStorage>(AdoptTag::ADOPT, result, size));
    }

    String intermediate()
    {
        return String(make<StringStorage>(_buffer, _used));
    }

    StringBuilder &append(String string)
    {
        return append(string.cstring());
    }

    StringBuilder &append(const char *str)
    {
        if (!str)
        {
            append("<null>");
        }
        else
        {
            for (size_t i = 0; str[i]; i++)
            {
                append(str[i]);
            }
        }

        return *this;
    }

    StringBuilder &append(const char *str, size_t size)
    {
        if (!str)
        {
            append("<null>");
        }
        else
        {
            for (size_t i = 0; i < size; i++)
            {
                append(str[i]);
            }
        }

        return *this;
    }

    StringBuilder &append(char chr)
    {
        if (_size == 0)
        {
            _buffer = new char[16];
            _size = 16;
            _used = 0;
        }

        if (_used + 1 == _size)
        {
            _size += _size / 4;
            _buffer = (char *)realloc(_buffer, _size);
        }

        _buffer[_used] = chr;
        _buffer[_used + 1] = '\0';
        _used++;

        return *this;
    }

    StringBuilder &rewind(size_t how_many)
    {
        assert(_used >= how_many);

        _used -= how_many;
        _buffer[_used] = '\0';

        return *this;
    }
};
