#pragma once

#include <libio/Formatter.h>
#include <libio/MemoryReader.h>
#include <libio/MemoryWriter.h>
#include <libio/Path.h>
#include <libio/Scanner.h>
#include <libio/Write.h>
#include <libutils/String.h>

namespace IO
{

static inline ResultOr<size_t> format(IO::Writer &writer, Scanner &scan)
{
    size_t written = 0;

    while (!scan.ended())
    {
        written += TRY(IO::write(writer, scan.next()));
    }

    return written;
}

template <typename First, typename... Args>
static inline ResultOr<size_t> format(Writer &writer, Scanner &scan, First first, Args... args)
{
    size_t written = 0;

    while (!scan.ended() && scan.peek() != '{')
    {
        written += TRY(IO::write(writer, scan.next()));
    }

    if (scan.peek() == '{')
    {
        auto fmt = Formatter::parse(scan);
        written += TRY(fmt.format(writer, first));
    }

    if (!scan.ended())
    {
        written += TRY(format(writer, scan, std::forward<Args>(args)...));
    }

    return written;
}

template <typename... Args>
static inline ResultOr<size_t> format(Writer &writer, const char *fmt, Args... args)
{
    MemoryReader memory{fmt};
    Scanner scan{memory};
    return format(writer, scan, std::forward<Args>(args)...);
}

static inline ResultOr<size_t> format(Writer &writer, const char *fmt)
{
    return IO::write(writer, fmt);
}

template <typename... Args>
static inline String format(const char *fmt, Args... args)
{
    MemoryWriter memory{};
    format(memory, fmt, std::forward<Args>(args)...);
    return memory.string();
}

} // namespace IO
