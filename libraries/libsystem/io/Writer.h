#pragma once

#include <libsystem/Common.h>
#include <libutils/RefPtr.h>

class Reader;
class SeekableReader;
class Writer
{
public:
    virtual size_t length() = 0;
    virtual size_t position() = 0;

    virtual void flush() = 0;

    virtual size_t write(const void *buffer, size_t size) = 0;
    virtual size_t write_byte(uint8_t v) { return write(&v, 1); }

    void copy_from(Reader &reader);
    void copy_from(SeekableReader &reader);
};