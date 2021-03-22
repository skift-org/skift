#pragma once
#include <libio/Reader.h>
#include <libsystem/math/MinMax.h>

namespace IO
{

template <IO::SeekableReader R>
class ScopedReader final : public Reader, public Seek
{
public:
    inline ScopedReader(R &reader, size_t size) : _reader(reader)
    {
        assert((size_t)(reader.length().value() - reader.tell().value()) > size);
        _start_pos = reader.tell().value();
        size_t rem_size = reader.length().value() - _start_pos;
        _size = MIN(rem_size, size);
    }

    inline ResultOr<size_t> length() override
    {
        return _size;
    }

    inline ResultOr<size_t> tell() override
    {
        return _reader.tell().value() - _start_pos;
    }

    inline ResultOr<size_t> read(void *buffer, size_t size) override
    {
        size_t rem_size = length().value() - tell().value();
        return _reader.read(buffer, MIN(rem_size, size));
    }

    inline ResultOr<size_t> seek(SeekFrom from) override
    {
        switch (from.whence)
        {
        case Whence::START:
        {
            auto offset = _start_pos + from.position;

            offset = MAX(_start_pos, offset);
            offset = MIN(_start_pos + _size, offset);

            return _reader.seek(SeekFrom::start(offset));
        }

        case Whence::CURRENT:
        {
            auto offset = _reader.tell().value() + from.position;

            offset = MAX(_start_pos, offset);
            offset = MIN(_start_pos + _size, offset);

            return _reader.seek(SeekFrom::start(from.position));
        }

        case Whence::END:
        {
            auto offset = _reader.length().value() + from.position;

            offset = MAX(_start_pos, offset);
            offset = MIN(_start_pos + _size, offset);

            return _reader.seek(SeekFrom::end(from.position));
        }

        default:
            ASSERT_NOT_REACHED();
        }
    }

private:
    size_t _start_pos = 0;
    size_t _size = 0;
    R &_reader;
};
} // namespace IO