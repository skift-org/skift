#pragma once

#include <libio/MemoryReader.h>
#include <libio/NumberScanner.h>
#include <libio/Scanner.h>
#include <libio/ScopedReader.h>
#include <libjson/Value.h>
#include <libutils/StringBuilder.h>
#include <libutils/Strings.h>

namespace Json
{

Value value(IO::Scanner &scan);

inline void whitespace(IO::Scanner &scan)
{
    scan.eat(Strings::WHITESPACE);
}

inline Value number(IO::Scanner &scan)
{
#ifdef __KERNEL__
    return IO::NumberScanner::decimal().scan_int(scan).unwrap_or(0);
#else
    return IO::NumberScanner::decimal().scan_float(scan).unwrap_or(0);
#endif
}

static inline const char *escape_sequence(IO::Scanner &scan)
{
    scan.skip('\\');

    if (scan.ended())
    {
        return "\\";
    }

    char chr = scan.current();
    scan.forward();

    switch (chr)
    {
    case '"':
        return "\"";

    case '\\':
        return "\\";

    case '/':
        return "/";

    case 'b':
        return "\b";

    case 'f':
        return "\f";

    case 'n':
        return "\n";

    case 'r':
        return "\r";

    case 't':
        return "\t";

    case 'u':
    {
        auto read_4hex = [&]() {
            char buffer[5];
            for (size_t i = 0; i < 4 && scan.current_is(Strings::LOWERCASE_XDIGITS); i++)
            {
                buffer[i] = scan.current();
                scan.forward();
            }

            IO::MemoryReader memory{buffer, 5};
            IO::Scanner bufscan{memory};
            return IO::NumberScanner::hexadecimal().scan_uint(bufscan).unwrap_or(0);
        };

        uint32_t first_surrogate = read_4hex();

        if (first_surrogate >= 0xDC00 && first_surrogate <= 0xDFFF)
        {
            // FIXME: We should use char8_t
            return reinterpret_cast<const char *>(u8"�");
        }

        if (!(first_surrogate >= 0xD800 && first_surrogate <= 0xDBFF))
        {
            // Not an UTF16 surrogate pair.
            static uint8_t utf8[5] = {};
            codepoint_to_utf8(first_surrogate, utf8);
            return (char *)utf8;
        }

        if (!scan.skip_word("\\u"))
        {
            // FIXME: We should use char8_t
            return reinterpret_cast<const char *>(u8"�");
        }

        uint32_t second_surrogate = read_4hex();

        if ((second_surrogate < 0xDC00) || (second_surrogate > 0xDFFF))
        {
            // Invalid second half of the surrogate pair
            // FIXME: We should use char8_t
            return reinterpret_cast<const char *>(u8"�");
        }

        Codepoint codepoint = 0x10000 + (((first_surrogate & 0x3FF) << 10) | (second_surrogate & 0x3FF));

        static uint8_t utf8[5] = {};
        codepoint_to_utf8(codepoint, utf8);
        return (char *)utf8;
    }

    default:
        break;
    }

    static char buffer[3] = "\\x";
    buffer[1] = chr;

    return buffer;
}

inline String string(IO::Scanner &scan)
{
    StringBuilder builder{};

    scan.skip('"');

    while (scan.current() != '"' && scan.do_continue())
    {
        if (scan.current() == '\\')
        {
            builder.append(escape_sequence(scan));
        }
        else
        {
            builder.append(scan.current());
            scan.forward();
        }
    }

    scan.skip('"');

    return builder.finalize();
}

inline Value array(IO::Scanner &scan)
{
    scan.skip('[');

    Value::Array array{};

    whitespace(scan);

    if (scan.skip(']'))
    {
        return move(array);
    }

    int index = 0;

    do
    {
        scan.skip(',');
        array.push_back(value(scan));
        index++;
    } while (scan.current() == ',');

    scan.skip(']');

    return move(array);
}

inline Value object(IO::Scanner &scan)
{
    scan.skip('{');

    Value::Object object{};

    whitespace(scan);

    if (scan.skip('}'))
    {
        return object;
    }

    while (scan.current() != '}')
    {
        auto k = string(scan);
        whitespace(scan);

        scan.skip(':');

        object[k] = value(scan);

        scan.skip(',');

        whitespace(scan);
    }

    scan.skip('}');

    return object;
}

inline Value keyword(IO::Scanner &scan)
{
    StringBuilder builder{};

    while (scan.current_is(Strings::LOWERCASE_ALPHA) &&
           scan.do_continue())
    {
        builder.append(scan.current());
        scan.forward();
    }

    auto keyword = builder.finalize();

    if (keyword == "true")
    {
        return true;
    }

    if (keyword == "false")
    {
        return false;
    }

    return nullptr;
}

inline Value value(IO::Scanner &scan)
{
    whitespace(scan);

    Value value{};

    if (scan.current() == '"')
    {
        value = string(scan);
    }
    else if (scan.current_is("-") ||
             scan.current_is("0123456789"))
    {
        value = number(scan);
    }
    else if (scan.current() == '{')
    {
        value = object(scan);
    }
    else if (scan.current() == '[')
    {
        value = array(scan);
    }
    else
    {
        value = keyword(scan);
    }

    whitespace(scan);

    return value;
}

inline Value parse(IO::Reader &reader)
{
    IO::Scanner scan{reader};
    scan.skip_utf8bom();
    return value(scan);
}

inline Value parse(const char *buffer, size_t size)
{
    IO::MemoryReader memory{buffer, size};
    return parse(memory);
}

inline Value parse(String &str)
{
    IO::MemoryReader memory{str};
    return parse(memory);
}

} // namespace Json