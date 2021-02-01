#include <libsystem/compression/DeflateReader.h>
#include <libsystem/io/MemoryReader.h>
#include <libsystem/io/MemoryWriter.h>
#include <libsystem/io/ScopedReader.h>

DeflateReader::DeflateReader(Reader &reader) : _reader(reader)
{
    // TODO: we shouldn't decompress everything at once
    _inflate.perform(reader, _mem_buffer);
}

size_t DeflateReader::length()
{
    return _mem_buffer.length();
}

size_t DeflateReader::position()
{
    return _position;
}

size_t DeflateReader::read(void *buffer, size_t size)
{
    // Do inflate until we have enough data or no more uncompressed data
    size_t remaining = MIN(length() - position(), size);

    memcpy(buffer, _mem_buffer.data().raw_storage() + _position, remaining);
    _position += remaining;

    return remaining;
}