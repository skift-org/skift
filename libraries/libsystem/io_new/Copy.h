#pragma once

#include <libutils/Array.h>
#include <libutils/Slice.h>

#include <libsystem/io_new/MemoryWriter.h>
#include <libsystem/io_new/Reader.h>
#include <libsystem/io_new/Writer.h>

namespace System
{

Result copy(Reader &from, Writer &to)
{
    constexpr int COPY_CHUNK_SIZE = 4096;

    do
    {
        Array<uint8_t, COPY_CHUNK_SIZE> copy_chunk;

        auto result_or_read = from.read(copy_chunk.raw_storage(), copy_chunk.count());

        if (!result_or_read)
        {
            return result_or_read.result();
        }

        if (*result_or_read == 0)
        {
            to.flush();
            return SUCCESS;
        }

        size_t chunk_size = *result_or_read;
        auto result_or_written = to.write(copy_chunk.raw_storage(), chunk_size);

        if (!result_or_written)
        {
            return result_or_written.result();
        }

        if (*result_or_written == 0)
        {
            to.flush();
            return SUCCESS;
        }
    } while (1);
}

ResultOr<Slice> read_all(Reader &reader)
{
    MemoryWriter memory;

    auto copy_result = copy(reader, memory);

    if (copy_result != SUCCESS)
    {
        return copy_result;
    }

    return Slice{memory.slice()};
}
//
// Result write_all(Writer &writer, Slice data)
// {
// }

} // namespace System
