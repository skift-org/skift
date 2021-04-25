#pragma once

#include <libio/Reader.h>
#include <libmath/MinMax.h>
#include <libutils/Slice.h>

namespace IO
{

class MemoryReader final :
    public Reader,
    public Seek
{
private:
    Slice _memory;
    size_t _position = 0;

public:
    MemoryReader(const char *cstring)
        : _memory{cstring}
    {
    }

    MemoryReader(const void *ptr, size_t size)
        : _memory(ptr, size)
    {
    }

    MemoryReader(Slice memory)
        : _memory{memory}
    {
    }

    MemoryReader(RawStorage &raw)
        : _memory(raw.storage())
    {
    }

    ResultOr<size_t> read(void *buffer, size_t size) override
    {
        if (_position > _memory.size())
        {
            return 0;
        }

        size_t remaining = MIN(_memory.size() - _position, size);

        memcpy(buffer, ((uint8_t *)_memory.start()) + _position, remaining);
        _position += remaining;

        return remaining;
    }

    ResultOr<size_t> seek(SeekFrom from) override
    {
        switch (from.whence)
        {
        case Whence::START:
            return _position = from.position;

        case Whence::CURRENT:
            return _position += from.position;

        case Whence::END:
            return _position = _memory.size() + from.position;

        default:
            ASSERT_NOT_REACHED();
        }
    }

    ResultOr<size_t> length() override
    {
        return _memory.size();
    }

    ResultOr<size_t> tell() override
    {
        return _position;
    }
};

} // namespace IO
