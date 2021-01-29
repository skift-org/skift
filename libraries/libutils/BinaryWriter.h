#pragma once
#include <libsystem/Common.h>
#include <libutils/Vector.h>

class BinaryWriter
{
public:
    BinaryWriter(Vector<uint8_t> &data) : _data(data)
    {
    }

    template <typename T>
    inline void put(const T &data)
    {
        uint8_t *bytes = (uint8_t *)&data;
        put_data(bytes, sizeof(T));
    }

    inline void put_data(uint8_t *data, unsigned int len)
    {
        _data.push_back_many({ADOPT, data, len});
    }

    inline void put_fixed_len_string(const String &str)
    {
        put_data((uint8_t *)str.cstring(), str.length());
    }

    inline unsigned int pos()
    {
        return _data.count();
    }

private:
    Vector<uint8_t> &_data;
};