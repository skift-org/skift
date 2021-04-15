#pragma once

#include <libio/MemoryReader.h>
#include <libio/MemoryWriter.h>
#include <libio/Scanner.h>
#include <libio/Write.h>
#include <libio/Path.h>
#include <libutils/String.h>

namespace IO
{

struct Formating
{
    enum Align
    {
        LEFT,
        CENTER,
        RIGHT,
    };

    enum Type
    {
        DEFAULT,
        STRING,
        CHARACTER,
        BINARY,
        DECIMAL,
        OCTAL,
        HEXADECIMAL,
    };

    bool prefix;
    Type type = DEFAULT;

    static Formating parse(Scanner &scan)
    {
        Formating format{};

        scan.skip('{');

        auto parse_type = [&]() {
            switch (scan.current())
            {
            case 's':
                format.type = STRING;
                break;

            case 'c':
                format.type = CHARACTER;
                break;

            case 'b':
                format.type = BINARY;
                break;

            case 'd':
                format.type = DECIMAL;
                break;

            case 'o':
                format.type = OCTAL;
                break;

            case 'x':
                format.type = HEXADECIMAL;
                break;

            default:
                break;
            }
        };

        if (scan.current_is("scbdox"))
        {
            parse_type();
            scan.forward();
        }

        while (!scan.ended() && scan.current() != '}')
        {
            scan.forward();
        }

        scan.skip('}');

        return format;
    }
};

struct Format
{
    virtual ~Format() {}
    virtual ResultOr<size_t> format(IO::Writer &writer);
};

template <typename T>
concept Formatable = IsBaseOf<Format, T>::value || requires(IO::Writer &writer, const Formating &formating, const T &t)
{
    format(writer, formating, t);
};

ResultOr<size_t> format(IO::Writer &, const Formating &, char);

ResultOr<size_t> format(IO::Writer &, const Formating &, unsigned char);

ResultOr<size_t> format(IO::Writer &, const Formating &, short int);

ResultOr<size_t> format(IO::Writer &, const Formating &, unsigned short int);

ResultOr<size_t> format(IO::Writer &, const Formating &, int);

ResultOr<size_t> format(IO::Writer &, const Formating &, unsigned int);

ResultOr<size_t> format(IO::Writer &, const Formating &, long int);

ResultOr<size_t> format(IO::Writer &, const Formating &, unsigned long int);

ResultOr<size_t> format(IO::Writer &, const Formating &, long long int);

ResultOr<size_t> format(IO::Writer &, const Formating &, unsigned long long int);

ResultOr<size_t> format(IO::Writer &, const Formating &, float);

ResultOr<size_t> format(IO::Writer &, const Formating &, double);

ResultOr<size_t> format(IO::Writer &, const Formating &, const char *);

template <typename T>
ResultOr<size_t> format(IO::Writer &writer, const Formating &format, const T *ptr)
{
    return IO::format(writer, format, reinterpret_cast<uintptr_t>(ptr));
}

ResultOr<size_t> format(IO::Writer &, const Formating &, const String);

ResultOr<size_t> format(IO::Writer &, const Formating &, const IO::Path);

static inline ResultOr<size_t> format(IO::Writer &writer, Scanner &scan)
{
    size_t written = 0;

    while (!scan.ended())
    {
        written += TRY(IO::write(writer, scan.current()));
        scan.forward();
    }

    return written;
}

template <Formatable First, Formatable... Args>
static inline ResultOr<size_t> format(Writer &writer, Scanner &scan, First first, Args... args)
{
    size_t written = 0;

    while (!scan.ended() && scan.current() != '{')
    {
        written += TRY(IO::write(writer, scan.current()));
        scan.forward();
    }

    if (scan.current() == '{')
    {
        auto formating = Formating::parse(scan);

        auto format_proxy = [&]() {
            if constexpr (IsBaseOf<Format, First>::value)
            {
                return first.format(writer);
            }
            else if constexpr (requires(const First &t) {
                                   format(writer, formating, t);
                               })
            {
                return format(writer, formating, first);
            }
            else
            {
                ASSERT_NOT_REACHED();
            }
        };

        written += TRY(format_proxy());
    }

    if (!scan.ended())
    {
        written += TRY(format(writer, scan, forward<Args>(args)...));
    }

    return written;
}

template <Formatable... Args>
static inline ResultOr<size_t> format(Writer &writer, const char *fmt, Args... args)
{
    MemoryReader memory{fmt};
    Scanner scan{memory};
    return format(writer, scan, forward<Args>(args)...);
}

static inline ResultOr<size_t> format(Writer &writer, const char *fmt)
{
    return IO::write(writer, fmt);
}

template <Formatable... Args>
static inline String format(const char *fmt, Args... args)
{
    MemoryWriter memory{};
    format(memory, fmt, forward<Args>(args)...);
    return memory.string();
}

} // namespace IO
