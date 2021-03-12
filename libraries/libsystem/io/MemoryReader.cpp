#include <libsystem/io/MemoryReader.h>

MemoryReader::MemoryReader(const uint8_t *data, size_t size) : _data(data), _size(size)
{
}

MemoryReader::MemoryReader(const Vector<uint8_t> &buffer) : MemoryReader(buffer.raw_storage(), buffer.count())
{
}

size_t MemoryReader::length()
{
    return _size;
}

size_t MemoryReader::position()
{
    return _position;
}

size_t MemoryReader::seek(IO::SeekFrom from)
{
    switch (from.whence)
    {
    case IO::Whence::START:
        _position = from.position;
        break;
    case IO::Whence::CURRENT:
        _position += from.position;
        break;
    case IO::Whence::END:
        _position = _size + from.position;
        break;
    default:
        ASSERT_NOT_REACHED();
        break;
    }

    return _position;
}

size_t MemoryReader::read(void *buffer, size_t size)
{
    size_t remaining = MIN(length() - position(), size);

    memcpy(buffer, _data + _position, remaining);
    _position += remaining;

    return remaining;
}