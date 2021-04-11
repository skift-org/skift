#pragma once

#ifndef __KERNEL__
#    include <math.h>
#endif

#include <libutils/Scanner.h>
#include <libutils/Strings.h>

static inline const char *scan_json_escape_sequence(Scanner &scan)
{
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
            for (size_t i = 0; i < 4 && scan.current_is(Strings::LOWERCASE_XDIGITS); i++)
            {
                buffer[i] = scan.current();
                scan.foreward();
            }

            uint32_t value = 0;
            parse_uint(PARSER_HEXADECIMAL, buffer, 5, &value);
            return value;
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

static inline unsigned int scan_uint(Scanner &scan, int base)
{
    assert(base >= 2 && base <= 16);

    unsigned int v = 0;
    while (scan.current_is(Strings::LOWERCASE_XDIGITS, base))
    {
        v *= base;
        v += scan.current() - '0';
        scan.foreward();
    }

    return v;
}

static inline int scan_int(Scanner &scan, int base)
{
    assert(base >= 2 && base <= 16);

    int sign = 1;

    if (scan.current_is("-"))
    {
        sign = -1;
    }

    scan.skip("+-");

    int digits = 0;

    while (scan.current_is(Strings::LOWERCASE_XDIGITS, base))
    {
        digits *= base;
        digits += scan.current() - '0';
        scan.foreward();
    }

    return digits * sign;
}

#ifndef __KERNEL__

static inline double scan_float(Scanner &scan)
{
    int ipart = scan_int(scan, 10);

    double fpart = 0;

    if (scan.skip('.'))
    {
        double multiplier = 0.1;

        while (scan.current_is(Strings::DIGITS))
        {
            fpart += multiplier * (scan.current() - '0');
            multiplier *= 0.1;
            scan.foreward();
        }
    }

    int exp = 0;

    if (scan.current_is("eE"))
    {
        scan.foreward();
        exp = scan_int(scan, 10);
    }

    return (ipart + fpart) * pow(10, exp);
}

#endif

static inline void scan_skip_utf8bom(Scanner &scan)
{
    scan.skip_word("\xEF\xBB\xBF");
}
