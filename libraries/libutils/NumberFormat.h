#pragma once

#include <math.h>
#include <string.h>

#include <libio/Writer.h>
#include <libutils/Strings.h>

struct NumberFormat
{
    int _base;
    int _precision;
    bool _capitalized;

    static NumberFormat binary()
    {
        return {2, 4, false};
    }

    static NumberFormat octal()
    {
        return {8, 4, false};
    }

    static NumberFormat decimal()
    {
        return {10, 4, false};
    }

    static NumberFormat hexadecimal()
    {
        return {16, 4, false};
    }

    NumberFormat capitalized()
    {
        NumberFormat copy = *this;
        copy._capitalized = true;
        return copy;
    }

    ResultOr<size_t> format(IO::Writer &writer, int64_t value)
    {
        size_t written = 0;

        if (value < 0)
        {
            written += TRY(IO::write(writer, '-'));
            value = -value;
        }

        return written + TRY(format(writer, (uint64_t)value));
    }

    ResultOr<size_t> format(IO::Writer &writer, uint64_t value)
    {
        if (value == 0)
        {
            return IO::write(writer, '0');
        }

        size_t i = 0;
        char buffer[64] = {};

        while (value != 0)
        {
            if (_capitalized)
            {
                buffer[i] = Strings::LOWERCASE_XDIGITS[value % _base];
            }
            else
            {
                buffer[i] = Strings::UPPERCASE_XDIGITS[value % _base];
            }

            value /= _base;
            i++;
        }

        strrvs(buffer);

        return writer.write(buffer, i);
    }

    ResultOr<size_t> format(IO::Writer &writer, float value)
    {
        return format(writer, (double)value);
    }

    ResultOr<size_t> format(IO::Writer &writer, double value)
    {
        size_t written = 0;

        int64_t ipart = (int64_t)value;
        double fpart = value - (double)ipart;

        written += TRY(format(writer, ipart));

        if (_precision > 0)
        {
            written += TRY(IO::write(writer, '.'));

            int64_t ifpart = fpart * pow(_base, _precision);

            written += TRY(format(writer, ifpart));
        }

        return written;
    }
};
