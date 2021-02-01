#pragma once
#include <libsystem/Common.h>
#include <libsystem/io/Writer.h>
#include <libutils/String.h>

class BinaryWriter
{
public:
    BinaryWriter(Writer &writer) : _writer(writer)
    {
    }

    template <typename T>
    inline void put(const T &data)
    {
        uint8_t *bytes = (uint8_t *)&data;
        put_data(bytes, sizeof(T));
    }

    inline void put_data(uint8_t *data, size_t len)
    {
        _writer.write(data, len);
    }

    inline void put_fixed_len_string(const String &str)
    {
        put_data((uint8_t *)str.cstring(), str.length());
    }

    inline size_t position()
    {
        return _writer.length();
    }

private:
    Writer &_writer;
};