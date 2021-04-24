#pragma once

#include <libio/Writer.h>
#include <libutils/Array.h>

namespace IO
{

class BufLine : public IO::Writer
{
private:
    IO::Writer &_writer;
    Array<uint8_t, 512> _buffer;
    size_t _used = 0;

public:
    BufLine(IO::Writer &writer) : _writer{writer} {}

    ~BufLine() { flush(); }

    ResultOr<size_t> write(const void *buffer, size_t size) override
    {
        for (size_t i = 0; i < size; i++)
        {
            uint8_t byte = static_cast<const uint8_t *>(buffer)[i];

            _buffer[_used] = byte;
            _used++;

            if (byte == '\n' || _used == _buffer.count())
            {
                TRY(flush());
            }
        }

        return size;
    }

    Result flush() override
    {
        auto result = _writer.write(_buffer.raw_storage(), _used).result();
        _used = 0;
        return result;
    }
};

} // namespace IO
