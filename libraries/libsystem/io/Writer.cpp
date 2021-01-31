#include <libsystem/io/Reader.h>
#include <libsystem/io/SeekableReader.h>
#include <libsystem/io/Writer.h>

#define COPY_CHUNK_SIZE 4096

void Writer::copy_from(Reader &reader)
{
    assert(reader.position() == 0);

    uint8_t copy_chunk[COPY_CHUNK_SIZE];
    size_t chunk_size = 0;

    while ((chunk_size = reader.read(copy_chunk, COPY_CHUNK_SIZE)))
    {
        write(copy_chunk, chunk_size);
    }
}

void Writer::copy_from(SeekableReader &reader)
{
    reader.seek(0, WHENCE_START);

    uint8_t copy_chunk[COPY_CHUNK_SIZE];
    size_t chunk_size = 0;

    while ((chunk_size = reader.read(copy_chunk, COPY_CHUNK_SIZE)))
    {
        write(copy_chunk, chunk_size);
    }
}