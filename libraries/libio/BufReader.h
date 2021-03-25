#pragma once

#include <string.h>

#include <libio/Reader.h>
#include <libsystem/math/MinMax.h>

namespace IO
{

class BufReader : public Reader
{
private:
    Reader &_reader;

    uint8_t *_buffer;
    size_t _used = 0;
    size_t _head = 0;
    size_t _size = 0;

    ResultOr<size_t> fill()
    {
        _used = TRY(_reader.read(_buffer, _size));
        _head = 0;

        return _used;
    }

    __noncopyable(BufReader);
    __nonmovable(BufReader);

public:
    BufReader(Reader &reader, size_t size)
        : _reader{reader}, _size{size}
    {
        _buffer = new uint8_t[_size];
    }

    ~BufReader()
    {
        delete _buffer;
    }

    ResultOr<size_t> read(void *buffer, size_t size) override
    {
        size_t data_left = size;
        uint8_t *data_to_read = (uint8_t *)buffer;

        while (data_left != 0)
        {
            // Fill the buffer with data
            if (_head == _used)
            {
                if (TRY(fill()) == 0)
                {
                    // Look like we have no more data to read
                    return size - data_left;
                }
            }

            // How many data can we copy from the buffer
            size_t used_space = _used - _head;
            size_t data_added = MIN(used_space, data_left);

            // Copy the data from the buffer
            memcpy(
                data_to_read,
                ((uint8_t *)_buffer) + _head,
                data_added);

            // Update the amount read
            data_left -= data_added;
            _head += data_added;
            data_to_read += data_added;
        }

        return size - data_left;
    }
};

} // namespace IO
