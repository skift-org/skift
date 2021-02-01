#include <libsystem/io/ScopedReader.h>

ScopedReader::ScopedReader(Reader &reader, size_t size) : _reader(reader)
{
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
    return _reader.read(buffer, size);
}