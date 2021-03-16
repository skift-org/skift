
#pragma once

#include <string.h>

#include <libutils/Storage.h>
#include <libutils/Tags.h>

class StringStorage final :
    public Storage
{
private:
    char *_buffer;
    size_t _length;

public:
    using Storage::end;
    using Storage::start;

    const char *cstring() { return _buffer; }

    void *start() override { return _buffer; }

    void *end() override { return reinterpret_cast<char *>(start()) + _length; }

    StringStorage(CopyTag, const char *cstring)
        : StringStorage(COPY, cstring, strlen(cstring))
    {
    }

    StringStorage(CopyTag, const char *cstring, size_t length)
    {
        _length = strnlen(cstring, length);
        _buffer = (char *)malloc(_length + 1);
        memcpy(_buffer, cstring, _length);
        _buffer[_length] = '\0';
    }

    StringStorage(AdoptTag, char *cstring)
        : StringStorage(ADOPT, cstring, strlen(cstring))
    {
    }

    StringStorage(AdoptTag, char *buffer, size_t length)
    {
        _length = length;
        _buffer = buffer;
    }

    ~StringStorage()
    {
        free(_buffer);
    }
};