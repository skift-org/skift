#include <libsystem/io/Reader.h>
#include <libsystem/io/SeekableReader.h>
#include <libsystem/io/Writer.h>
#include <libutils/Array.h>

#define COPY_CHUNK_SIZE 4096

void Writer::copy_from(Reader &reader)
{
    assert(reader.position() == 0);

    Array<uint8_t, COPY_CHUNK_SIZE> copy_chunk;
    size_t chunk_size = 0;

    while ((chunk_size = reader.read(copy_chunk.raw_storage(), copy_chunk.count())))
    {
        write(copy_chunk.raw_storage(), chunk_size);
    }

    flush();
}

void Writer::copy_from(SeekableReader &reader)
{
    reader.seek(0, WHENCE_START);

    Array<uint8_t, COPY_CHUNK_SIZE> copy_chunk;
    size_t chunk_size = 0;

    while ((chunk_size = reader.read(copy_chunk.raw_storage(), copy_chunk.count())))
    {
        write(copy_chunk.raw_storage(), chunk_size);
    }

    flush();
}