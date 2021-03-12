#pragma once

#include <libio/Writer.h>
#include <libsystem/math/MinMax.h>
#include <libutils/SliceStorage.h>
#include <libutils/StringStorage.h>

namespace IO
{

class MemoryWriter :
    public Writer
{
private:
    size_t _used = 0;
    size_t _size = 0;
    uint8_t *_buffer = nullptr;

public:
    using Writer::flush;

    MemoryWriter() : MemoryWriter(16)
    {
    }

    MemoryWriter(size_t preallocated)
    {
        preallocated = MAX(preallocated, 16);

        _buffer = new uint8_t[preallocated];
        _buffer[0] = '\0';
        _size = preallocated;
        _used = 0;
    }

    ~MemoryWriter()
    {
        if (_buffer)
        {
            delete[] _buffer;
        }
    }

    // Create a string and flush the buffer!;
    RefPtr<StringStorage> string()
    {
        write('\0'); // Null terminator

        uint8_t *result = _buffer;
        size_t size = _used;

        _buffer = nullptr;
        _used = 0;
        _size = 0;

        return make<StringStorage>(ADOPT, (char *)result, size - 1);
    }

    RefPtr<SliceStorage> slice()
    {
        uint8_t *result = _buffer;
        size_t size = _used;

        _buffer = nullptr;
        _used = 0;
        _size = 0;

        return make<SliceStorage>(ADOPT, (void *)result, size);
    }

    ResultOr<size_t> write(uint8_t v)
    {
        if (_size == 0)
        {
            _buffer = new uint8_t[16];
            _buffer[0] = '\0';
            _size = 16;
            _used = 0;
        }

        if (_used == _size)
        {
            auto new_size = _size + _size / 4;
            auto new_buffer = new uint8_t[new_size];
            memcpy(new_buffer, _buffer, _size);
            delete[] _buffer;

            _size = new_size;
            _buffer = new_buffer;
        }

        _buffer[_used] = v;
        _used++;

        return 1;
    }

    ResultOr<size_t> write(const void *buffer, size_t size) override
    {
        for (size_t i = 0; i < size; i++)
        {
            write(((uint8_t *)buffer)[i]);
        }

        return size;
    }
};

} // namespace IO
