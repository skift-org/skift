#include <libsystem/io/ScopedReader.h>

ScopedReader::ScopedReader(Reader &reader, size_t size) : _reader(reader)
{
    assert((reader.length() - reader.position()) > size);
    _start_pos = reader.position();
    _size = MIN(reader.length() - _start_pos, size);
}

size_t ScopedReader::length()
{
    return _size;
}

size_t ScopedReader::position()
{
    return _reader.position() - _start_pos;
}

size_t ScopedReader::read(void *buffer, size_t size)
{
    size_t rem_size = MIN(length() - position(), size);
    return _reader.read(buffer, rem_size);
}