#pragma once
#include <libsystem/Common.h>
#include <libsystem/io/Writer.h>
#include <libutils/String.h>

class BinaryWriter : public Writer
{
private:
    Writer &_writer;

public:
    BinaryWriter(Writer &writer) : _writer(writer)
    {
    }

    // Put functions
    template <typename T>
    inline void put(const T &data)
    {
        uint8_t *bytes = (uint8_t *)&data;
        write(bytes, sizeof(T));
    }

    inline void put_fixed_len_string(const String &str)
    {
        write((uint8_t *)str.cstring(), str.length());
    }

    // Inherited from Writer
    inline size_t position() override
    {
        return _writer.position();
    }

    inline size_t length() override
    {
        return _writer.length();
    }

    inline void flush() override
    {
        _writer.flush();
    }

    inline size_t write(const void *buffer, size_t size) override
    {
        return _writer.write(buffer, size);
    }
};