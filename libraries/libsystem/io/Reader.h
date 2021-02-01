#pragma once

#include <libsystem/Common.h>
#include <libutils/RefPtr.h>

class Writer;
class Reader
{
public:
    virtual size_t length() = 0;
    virtual size_t position() = 0;

    virtual size_t read(void *buffer, size_t size) = 0;

    virtual uint8_t read_byte()
    {
        uint8_t result;
        assert(read(&result, 1));
        return result;
    };

    virtual void read_all(void **buffer, size_t *size)
    {
        *buffer = new uint8_t[length()];
        *size = length();
        assert(read(*buffer, length()) == length());
    }

    virtual void copy_to(Writer &writer);
};