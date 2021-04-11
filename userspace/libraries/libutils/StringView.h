#pragma once

#include <string.h>

#include <libutils/RefPtr.h>
#include <libutils/StringStorage.h>

class StringView
{
private:
    RefPtr<StringStorage> _storage;
    const char *_buffer;
    size_t _size;

public:
    const char *buffer() const
    {
        return _buffer;
    }

    size_t size() const
    {
        return _size;
    }

    StringView(const char *buffer)
        : _buffer(buffer), _size(strlen(buffer))
    {
    }

    StringView(const char *buffer, size_t size)
        : _buffer(buffer), _size(size)
    {
    }

    ~StringView()
    {
    }
};