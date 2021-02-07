#pragma once

#include <libutils/RefCounted.h>
#include <string.h>

class StringStorage :
    public RefCounted<StringStorage>
{
private:
    size_t _length;
    char *_buffer;

public:
    const char *cstring() { return _buffer; }

    size_t length() { return _length; }

    StringStorage(const char *cstring)
        : StringStorage(cstring, strlen(cstring))
    {
    }

    StringStorage(const char *cstring, size_t length)
    {
        _length = strnlen(cstring, length);
        _buffer = new char[_length + 1];
        memcpy(_buffer, cstring, _length);
        _buffer[_length] = '\0';
    }

    StringStorage(AdoptTag, char *buffer, size_t length)
    {
        _length = length;
        _buffer = buffer;
    }

    StringStorage(StringStorage &left, StringStorage &right)
    {
        _length = left.length() + right.length();
        _buffer = new char[_length + 1];

        memcpy(_buffer, left.cstring(), left.length());
        memcpy(_buffer + left.length(), right.cstring(), right.length());

        _buffer[left.length() + right.length()] = '\0';
    }

    ~StringStorage()
    {
        delete[] _buffer;
    }
};
