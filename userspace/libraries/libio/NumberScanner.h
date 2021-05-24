#pragma once

#include <math.h>

#include <libio/Scanner.h>
#include <libutils/Strings.h>

namespace IO
{

struct NumberScanner
{
    int _base = 10;

public:
    static NumberScanner binary() { return {2}; }

    static NumberScanner octal() { return {8}; }

    static NumberScanner decimal() { return {10}; }

    static NumberScanner hexadecimal() { return {16}; }

    bool is_digit(Scanner &scan)
    {
        char c = scan.peek();

        for (int i = 0; i < _base; i++)
        {
            if ((Strings::LOWERCASE_XDIGITS[i] == c) ||
                (Strings::UPPERCASE_XDIGITS[i] == c))
            {
                return true;
            }
        }

        return false;
    }

    Optional<uint8_t> scan_digit(Scanner &scan)
    {
        char c = scan.peek();

        for (int i = 0; i < _base; i++)
        {
            if ((Strings::LOWERCASE_XDIGITS[i] == c) ||
                (Strings::UPPERCASE_XDIGITS[i] == c))
            {
                scan.next();
                return i;
            }
        }

        return NONE;
    }

    Optional<uint64_t> scan_uint(Scanner &scan)
    {
        if (!is_digit(scan))
        {
            return NONE;
        }

        uint64_t value = 0;

        while (!scan.ended() && is_digit(scan))
        {
            value = value * _base;
            value += scan_digit(scan).unwrap();
        }

        return value;
    }

    Optional<int64_t> scan_int(Scanner &scan)
    {
        if (!is_digit(scan) &&
            scan.peek() != '-')
        {
            return NONE;
        }

        bool is_negative = scan.skip('-');

        auto unsigned_value = scan_uint(scan);

        if (!unsigned_value.present())
        {
            return NONE;
        }

        if (is_negative)
        {
            return -unsigned_value.unwrap();
        }
        else
        {
            return unsigned_value.unwrap();
        }
    }

    Optional<double> scan_float(Scanner &scan)
    {
        int64_t ipart = scan_int(scan).unwrap_or(0);

        double fpart = 0;

        if (scan.skip('.'))
        {
            double multiplier = (1.0 / _base);

            while (is_digit(scan))
            {
                fpart += multiplier * scan_digit(scan).unwrap();
                multiplier *= (1.0 / _base);
            }
        }

        int64_t exp = 0;

        if (scan.peek_is_any("eE"))
        {
            scan.next();
            exp = scan_int(scan).unwrap_or(0);
        }

        return (ipart + fpart) * pow(_base, exp);
    }
};

} // namespace IO
