#include <libsystem/io/MemoryWriter.h>

MemoryWriter::MemoryWriter(size_t reserve) : _data(reserve)
{
}

size_t MemoryWriter::length()
{
    return _data.count();
}

size_t MemoryWriter::position()
{
    return _data.count();
}

void MemoryWriter::flush()
{
}

void MemoryWriter::clear()
{
    _data.clear();
}

size_t MemoryWriter::write(const void *buffer, size_t size)
{
    _data.push_back_data((const uint8_t *)buffer, size);
    return size;
}