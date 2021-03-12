#pragma once

#include <libio/Reader.h>
#include <libutils/Vector.h>

namespace IO
{

template <typename T>
ResultOr<size_t> read(Reader &reader, Vector<T> &vector)
{
    size_t read = 0;

    T object;
    size_t last_read = TRY(reader.read(&object, sizeof(T)));
    while (last_read > 0)
    {
        vector.push_back(move(object));
        read += last_read;

        last_read = TRY(reader.read(&object, sizeof(T)));
    }

    return read;
}

} // namespace IO
