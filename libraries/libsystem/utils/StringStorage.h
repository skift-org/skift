#pragma once

#include <libsystem/core/CString.h>
#include <libsystem/utils/RefCounted.h>

class StringStorage : public RefCounted<StringStorage>
{
private:
    size_t _length;
    char *_buffer;

public:
    const char *cstring() { return _buffer; }

    size_t length() { return _length; }

    StringStorage(const char *cstring) : StringStorage(cstring, strlen(cstring)){};

    StringStorage(const char *cstring, size_t length)
    {
        _length = length;
        _buffer = new char[length + 1];
        memcpy(_buffer, cstring, length);
        _buffer[length] = '\0';
    }

    ~StringStorage()
    {
        delete _buffer;
    }
};
