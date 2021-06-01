#pragma once

#include <libutils/Array.h>
#include <libutils/Slice.h>
#include <libutils/Vector.h>

#include <libio/MemoryReader.h>
#include <libio/MemoryWriter.h>

namespace IO
{

constexpr int COPY_CHUNK_SIZE = 4096;

static inline HjResult copy(Reader &from, Writer &to, size_t n)
{
    size_t remaining = n;

    do
    {
        Array<uint8_t, COPY_CHUNK_SIZE> copy_chunk;

        size_t read = TRY(from.read(copy_chunk.raw_storage(), MIN(COPY_CHUNK_SIZE, remaining)));

        if (read == 0)
        {
            to.flush();
            return SUCCESS;
        }

        size_t written = TRY(to.write(copy_chunk.raw_storage(), read));

        remaining -= read;

        if (written == 0 || remaining == 0)
        {
            to.flush();
            return SUCCESS;
        }
    } while (1);
}

static inline HjResult copy(Reader &from, Writer &to)
{
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

static inline ResultOr<Slice> read_all(Reader &reader)
{
    MemoryWriter memory;

    TRY(copy(reader, memory));

    return Slice{memory.slice()};
}

static inline ResultOr<Slice> read_all(MemoryReader &reader)
{
    if (TRY(reader.tell()) == 0)
    {
        return reader.memory();
    }
    else
    {
        return read_all((Reader &)reader);
    }
}

static inline HjResult write_all(Writer &writer, Slice data)
{
    MemoryReader memory{data};
    return copy(memory, writer);
}

static inline ResultOr<size_t> copy_line(Reader &from, Writer &to, char delimiter = '\n')
{
    size_t written = 0;

    char c;
    size_t read = TRY(from.read(&c, sizeof(c)));

    while (read > 0 && c != delimiter)
    {
        written += TRY(to.write(&c, sizeof(c)));
        TRY(from.read(&c, sizeof(c)));
    }

    if (c == delimiter)
    {
        written += TRY(to.write(&c, sizeof(c)));
    }

    return written;
}

static inline ResultOr<size_t> copy_line(Scanner &scan, Writer &to, String delimiter = "\n", bool write_delim = true)
{
    size_t written = 0;

    while (!scan.peek_is_word(delimiter.cstring()) &&
           !scan.ended())
    {
        written += TRY(IO::write(to, scan.next()));
    }

    if (scan.skip_word(delimiter.cstring()) && write_delim)
    {
        IO::write(to, delimiter);
    }

    return written;
}

static inline HjResult head(Reader &from, Writer &to, char delimiter = '\n', size_t n = 10)
{
    for (size_t i = 0; i < n; i++)
    {
        TRY(copy_line(from, to, delimiter));
    }

    return SUCCESS;
}

static inline HjResult tail(Reader &from, Writer &to, char delimiter = '\n', size_t n = 10)
{
    Vector<Slice> tail;

    auto append_line = [&](Slice line) {
        tail.push_back(line);
        if (tail.count() > n)
        {
            tail.pop();
        }
    };

    size_t copied = 0;
    MemoryWriter memory;

    do
    {
        copied = TRY(copy_line(from, memory, delimiter));
        append_line(Slice{memory.slice()});
    } while (copied > 0);

    for (auto line : tail)
    {
        TRY(write_all(to, line));
    }

    return SUCCESS;
}

} // namespace IO
