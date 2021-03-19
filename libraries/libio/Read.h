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

// Peek & Get functons
template <typename T, typename R>
requires SeekableReader<R> inline T peek(R &reader)
{
    T result = get<T>(reader);
    reader.seek(IO::SeekFrom::current(-sizeof(T)));
    return result;
}

template <typename T>
inline T get(Reader &reader)
{
    T result;
    assert(reader.read(&result, sizeof(T)) == sizeof(T));
    return result;
}

} // namespace IO
