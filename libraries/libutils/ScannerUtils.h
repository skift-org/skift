#pragma once

#include <libutils/Scanner.h>

static inline const char *scan_json_escape_sequence(Scanner &scan)
{
    constexpr auto XDIGITS = "0123456789abcdef";

    scan.skip('\\');

    if (scan.ended())
    {
        return "\\";
    }

    char chr = scan.current();
    scan.foreward();

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
            for (size_t i = 0; i < 4 && scan.current_is(XDIGITS); i++)
            {
                buffer[i] = scan.current();
                scan.foreward();
            }

            uint value = 0;
            parse_uint(PARSER_HEXADECIMAL, buffer, 5, (unsigned int *)&value);
            return value;
        };

        uint first_surrogate = read_4hex();

        if (first_surrogate >= 0xDC00 && first_surrogate <= 0xDFFF)
        {
            // FIXME: We should use char8_t
            return reinterpret_cast<const char *>(u8"�");
        }

        if (!(first_surrogate >= 0xD800 && first_surrogate <= 0xDBFF))
        {
            // Not an UTF16 surrogate pair.
            static uint8_t utf8[5] = {};
            codepoint_to_utf8((Codepoint)first_surrogate, utf8);
            return (char *)utf8;
        }

        if (!scan.skip_word("\\u"))
        {
            // FIXME: We should use char8_t
            return reinterpret_cast<const char *>(u8"�");
        }

        uint second_surrogate = read_4hex();

        if ((second_surrogate < 0xDC00) || (second_surrogate > 0xDFFF))
        {
            // Invalid second half of the surrogate pair
            // FIXME: We should use char8_t
            return reinterpret_cast<const char *>(u8"�");
        }

        Codepoint codepoint = 0x10000 + (((first_surrogate & 0x3FF) << 10) | (second_surrogate & 0x3FF));

        static uint8_t utf8[5] = {};
        codepoint_to_utf8((Codepoint)codepoint, utf8);
        return (char *)utf8;
    }

    default:
        break;
    }

    static char buffer[3] = "\\x";
    buffer[1] = chr;

    return buffer;
}

static inline void scan_skip_utf8bom(Scanner &scan)
{
    scan.skip_word("\xEF\xBB\xBF");
}
