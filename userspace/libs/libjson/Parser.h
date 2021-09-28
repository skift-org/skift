#pragma once

#include <libio/MemoryReader.h>
#include <libio/MemoryWriter.h>
#include <libio/NumberScanner.h>
#include <libio/Scanner.h>
#include <libio/ScopedReader.h>
#include <libio/Write.h>
#include <libjson/Value.h>
#include <libutils/Strings.h>

namespace Json
{

Value parse_value(IO::Scanner &scan);

inline void parse_whitespace(IO::Scanner &scan)
{
    scan.eat_any(Strings::WHITESPACE);
}

inline Value parse_number(IO::Scanner &scan)
{
#ifdef __KERNEL__
    return IO::NumberScanner::decimal().scan_int(scan).unwrap_or(0);
#else
    return IO::NumberScanner::decimal().scan_float(scan).unwrap_or(0);
#endif
}

static inline const char *parse_escape_sequence(IO::Scanner &scan)
{
    scan.skip('\\');

    if (scan.ended())
    {
        return "\\";
    }

    char chr = scan.next();

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
            for (size_t i = 0; i < 4 && scan.peek_is_any(Strings::LOWERCASE_XDIGITS); i++)
            {
                buffer[i] = scan.next();
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
            Text::rune_to_utf8(first_surrogate, utf8);
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

        Text::Rune rune = 0x10000 + (((first_surrogate & 0x3FF) << 10) | (second_surrogate & 0x3FF));

        static uint8_t utf8[5] = {};
        Text::rune_to_utf8(rune, utf8);
        return (char *)utf8;
    }

    default:
        break;
    }

    static char buffer[3] = "\\x";
    buffer[1] = chr;

    return buffer;
}

inline String parse_string(IO::Scanner &scan)
{
    IO::MemoryWriter builder{};

    scan.skip('"');

    while (scan.peek() != '"' &&
           !scan.ended())
    {
        if (scan.peek() == '\\')
        {
            IO::write(builder, parse_escape_sequence(scan));
        }
        else
        {
            IO::write(builder, scan.next());
        }
    }

    scan.skip('"');

    return builder.string();
}

inline Value parse_array(IO::Scanner &scan)
{
    scan.skip('[');

    Value::Array array{};

    parse_whitespace(scan);

    if (scan.skip(']'))
    {
        return array;
    }

    int index = 0;

    do
    {
        scan.skip(',');
        array.push_back(parse_value(scan));
        index++;
    } while (scan.peek() == ',');

    scan.skip(']');

    return array;
}

inline Value parse_object(IO::Scanner &scan)
{
    scan.skip('{');

    Value::Object object{};

    parse_whitespace(scan);

    if (scan.skip('}'))
    {
        return object;
    }

    while (scan.peek() != '}')
    {
        auto k = parse_string(scan);
        parse_whitespace(scan);

        scan.skip(':');

        object[k] = parse_value(scan);

        scan.skip(',');

        parse_whitespace(scan);
    }

    scan.skip('}');

    return object;
}

inline Value parse_value(IO::Scanner &scan)
{
    parse_whitespace(scan);

    Value value{};

    if (scan.peek() == '"')
    {
        value = parse_string(scan);
    }
    else if (scan.peek() == '-' ||
             scan.peek_is_any("0123456789"))
    {
        value = parse_number(scan);
    }
    else if (scan.peek() == '{')
    {
        value = parse_object(scan);
    }
    else if (scan.peek() == '[')
    {
        value = parse_array(scan);
    }
    else if (scan.skip_word("true"))
    {
        value = true;
    }
    else if (scan.skip_word("false"))
    {
        value = false;
    }
    else
    {
        value = nullptr;
    }

    parse_whitespace(scan);

    return value;
}

inline Value parse(IO::Reader &reader)
{
    IO::Scanner scan{reader};
    scan.skip_utf8bom();
    return parse_value(scan);
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