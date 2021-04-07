#pragma once

#include <assert.h>
#include <libsystem/Common.h>
#include <libsystem/math/MinMax.h>
#include <libutils/String.h>
#include <libutils/Vector.h>
#include <libutils/unicode/Codepoint.h>

class StringBuilder
{
private:
    size_t _used = 0;
    size_t _size = 0;
    char *_buffer = nullptr;

    NONCOPYABLE(StringBuilder);
    NONMOVABLE(StringBuilder);

public:
    size_t length() const
    {
        return _used;
    }

    StringBuilder() : StringBuilder(16)
    {
    }

    StringBuilder(size_t preallocated)
    {
        preallocated = MAX(preallocated, 16);

        _buffer = new char[preallocated];
        _buffer[0] = '\0';
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
        if (!_buffer)
        {
            return "";
        }

        char *result = _buffer;
        size_t size = _used;

        _buffer = nullptr;
        _used = 0;
        _size = 0;

        return String(make<StringStorage>(ADOPT, result, size));
    }

    String intermediate()
    {
        return String(make<StringStorage>(COPY, _buffer, _used));
    }

    StringBuilder &append(String string)
    {
        for (size_t i = 0; i < string.length(); i++)
        {
            append(string[i]);
        }

        return *this;
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

    StringBuilder &append_codepoint(Codepoint cp)
    {
        char buffer[5];
        codepoint_to_utf8(cp, (uint8_t *)buffer);
        append(buffer);

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
            auto new_size = _size + _size / 4;
            auto new_buffer = new char[new_size];
            memcpy(new_buffer, _buffer, _size);
            delete[] _buffer;

            _size = new_size;
            _buffer = new_buffer;
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
