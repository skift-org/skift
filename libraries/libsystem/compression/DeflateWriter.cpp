#include <libsystem/compression/DeflateWriter.h>
#include <libsystem/io/MemoryReader.h>
#include <libsystem/io/MemoryWriter.h>

#define MAX_BLOCK_SIZE 0xFFFF

DeflateWriter::DeflateWriter(Writer &writer, int level) : _deflate(level), _writer(writer)
{
}

DeflateWriter::~DeflateWriter()
{
    flush();
}

size_t DeflateWriter::length()
{
    return _writer.length();
}

size_t DeflateWriter::position()
{
    return _writer.position();
}

void DeflateWriter::flush()
{
    MemoryReader mem_reader(_mem_buffer.data());
    _deflate.perform(mem_reader, _writer);
    _mem_buffer.clear();
}

size_t DeflateWriter::write(const void *buffer, size_t size)
{
    size_t written = _mem_buffer.write(buffer, size);

    if (_mem_buffer.length() > MAX_BLOCK_SIZE)
    {
        flush();
    }

    return written;
}