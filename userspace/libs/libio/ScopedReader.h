#pragma once

#include <libio/Reader.h>
#include <libmath/MinMax.h>

namespace IO
{

struct ScopedReader :
    public Reader
{
private:
    Reader &_reader;
    size_t _count = 0;
    size_t _length = 0;

public:
    ScopedReader(Reader &reader, size_t length) : _reader{reader}, _length{length}
    {
    }

    inline ResultOr<size_t> length()
    {
        return _length;
    }

    ResultOr<size_t> read(void *buffer, size_t size) override
    {
        size_t to_read = MIN(_length - _count, size);
        auto result = TRY(_reader.read(buffer, to_read));
        _count += result;
        return result;
    }
};

} // namespace IO
