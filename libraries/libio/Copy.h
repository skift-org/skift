#pragma once

#include <libutils/Array.h>
#include <libutils/Slice.h>

#include <libio/MemoryReader.h>
#include <libio/MemoryWriter.h>
#include <libio/Reader.h>
#include <libio/Writer.h>

namespace IO
{

Result copy(Reader &from, Writer &to)
{
    constexpr int COPY_CHUNK_SIZE = 4096;

    do
    {
        Array<uint8_t, COPY_CHUNK_SIZE> copy_chunk;

        auto read = TRY(from.read(copy_chunk.raw_storage(), copy_chunk.count()));

        if (read == 0)
        {
            to.flush();
            return SUCCESS;
        }

        auto written = TRY(to.write(copy_chunk.raw_storage(), read));

        if (written == 0)
        {
            to.flush();
            return SUCCESS;
        }
    } while (1);
}

ResultOr<Slice> read_all(Reader &reader)
{
    MemoryWriter memory;

    TRY(copy(reader, memory));

    return Slice{memory.slice()};
}

Result write_all(Writer &writer, Slice data)
{
    MemoryReader memory{data};
    return copy(memory, writer);
}

} // namespace IO
