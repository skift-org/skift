#pragma once

#include <libio/MemoryReader.h>
#include <libio/MemoryWriter.h>
#include <libio/Scanner.h>
#include <libio/Writer.h>
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

    static Formating parse(Scanner &scanner)
    {
        Formating format{};

        scanner.skip('{');

        auto parse_type = [&]() {
            switch (scanner.current())
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

        if (scanner.current_is("scbdox"))
        {
            parse_type();
            scanner.forward();
        }

        while (!scanner.ended() && scanner.current() != '}')
        {

            scanner.forward();
        }

        scanner.skip('}');

        return format;
    }
};

class Format
{
public:
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

ResultOr<size_t> format(IO::Writer &, const Formating &, float);

ResultOr<size_t> format(IO::Writer &, const Formating &, double);

ResultOr<size_t> format(IO::Writer &, const Formating &, const char *);

ResultOr<size_t> format(IO::Writer &, const Formating &, const String);

static inline ResultOr<size_t> format(IO::Writer &writer, Scanner &scanner)
{
    size_t written = 0;

    while (!scanner.ended())
    {
        written += TRY(writer.write(scanner.current()));
        scanner.forward();
    }

    return written;
}

template <Formatable First, Formatable... Args>
static inline ResultOr<size_t> format(Writer &writer, Scanner &scanner, First first, Args... args)
{
    size_t written = 0;

    while (!(scanner.ended() || scanner.current() == '{'))
    {
        written += TRY(writer.write(scanner.current()));
        scanner.forward();
    }

    if (scanner.current() == '{')
    {
        auto formating = Formating::parse(scanner);

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

    if (!scanner.ended())
    {
        written += TRY(format(writer, scanner, forward<Args>(args)...));
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
    return writer.write(fmt);
}

template <Formatable... Args>
static inline String format(const char *fmt, Args... args)
{
    MemoryWriter memory{};
    format(memory, fmt, forward<Args>(args)...);
    return memory.string();
}

} // namespace IO
