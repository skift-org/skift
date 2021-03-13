#pragma once

#include <math.h>

#include <libio/Scanner.h>
#include <libutils/Strings.h>

namespace IO
{

struct NumberScanner
{
    int _base;

    static NumberScanner binary() { return {2}; }

    static NumberScanner octal() { return {8}; }

    static NumberScanner decimal() { return {10}; }

    static NumberScanner hexadecimal() { return {16}; }

    Optional<uint8_t> scan_digit(Scanner &scan)
    {
        if (!scan.current_is(Strings::ALL_XDIGITS))
        {
            return {};
        }

        char c = scan.current();

        for (int i = 0; i < _base; i++)
        {
            if ((Strings::LOWERCASE_XDIGITS[i] == c) ||
                (Strings::UPPERCASE_XDIGITS[i] == c))
            {
                scan.forward();
                return i;
            }
        }

        ASSERT_NOT_REACHED();
    }

    Optional<uint64_t> scan_uint(Scanner &scan)
    {
        if (!scan.current_is(Strings::ALL_XDIGITS))
        {
            return {};
        }

        uint64_t value = 0;

        while (!scan.ended() &&
               scan.current_is(Strings::ALL_XDIGITS))
        {
            value = value * _base;
            value += *scan_digit(scan);
        }

        return value;
    }

    Optional<int64_t> scan_int(Scanner &scan)
    {
        if (!scan.current_is(Strings::ALL_XDIGITS) &&
            !scan.current_is("-"))
        {
            return {};
        }

        bool is_negative = scan.skip('-');

        auto unsigned_value = scan_uint(scan);

        if (!unsigned_value)
        {
            return {};
        }

        if (is_negative)
        {
            return -*unsigned_value;
        }
        else
        {
            return *unsigned_value;
        }
    }

    Optional<double> scan_float(Scanner &scan)
    {
        int64_t ipart = scan_int(scan);

        double fpart = 0;

        if (scan.skip('.'))
        {
            double multiplier = (1.0 / _base);

            while (scan.current_is(Strings::ALL_XDIGITS))
            {
                fpart += multiplier * *scan_digit(scan);
                multiplier *= (1.0 / _base);
            }
        }

        int64_t exp = 0;

        if (scan.current_is("eE"))
        {
            scan.forward();
            exp = scan_int(scan);
        }

        return (ipart + fpart) * pow(_base, exp);
    }
};

} // namespace IO
