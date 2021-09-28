#pragma once

#include <libio/Reader.h>

namespace IO
{

struct ReadCounter :
    public Reader
{
private:
    size64_t _count = 0;
    Reader &_reader;

public:
    void reset()
    {
        _count = 0;
    }

    size64_t count()
    {
        return _count;
    }

    ReadCounter(Reader &reader) : _reader{reader}
    {
    }

    ResultOr<size_t> read(void *buffer, size_t size) override
    {
        auto result = TRY(_reader.read(buffer, size));
        _count += result;
        return result;
    }
};

} // namespace IO
