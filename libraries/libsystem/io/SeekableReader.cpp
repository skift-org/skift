#include <libsystem/io/SeekableReader.h>
#include <libsystem/io/Writer.h>

#define COPY_CHUNK_SIZE 4096

void SeekableReader::copy_to(Writer &writer)
{
    seek(0, WHENCE_START);

    uint8_t copy_chunk[COPY_CHUNK_SIZE];
    size_t chunk_size = 0;

    while ((chunk_size = read(copy_chunk, COPY_CHUNK_SIZE)))
    {
        writer.write(copy_chunk, chunk_size);
    }
}