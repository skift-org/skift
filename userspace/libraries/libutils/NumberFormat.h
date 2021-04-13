#pragma once

#include <math.h>
#include <string.h>

#include <libio/Write.h>
#include <libio/Writer.h>
#include <libutils/Strings.h>

class NumberFormat
{
private:
    int _base = 10;
    int _precision = 4;
    bool _capitalized = false;

    char digit(int digit)
    {
        assert(digit >= 0 && digit < _base);

        if (_capitalized)
        {
            return Strings::LOWERCASE_XDIGITS[digit % _base];
        }
        else
        {
            return Strings::UPPERCASE_XDIGITS[digit % _base];
        }
    }

public:
    static NumberFormat binary() { return {2, 4, false}; }
    static NumberFormat octal() { return {8, 4, false}; }
    static NumberFormat decimal() { return {10, 4, false}; }
    static NumberFormat hexadecimal() { return {16, 4, false}; }

    NumberFormat(int base, int precision, bool capitalized)
        : _base{base}, _precision{precision}, _capitalized{capitalized}
    {
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
            buffer[i] = digit(value % _base);
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
        if (isnan(value))
        {
            return IO::write(writer, _capitalized ? "NaN" : "nan");
        }

        size_t written = 0;

        if (value < 0)
        {
            written += TRY(IO::write(writer, '-'));
            value = -value;
        }

        if (isinf(value))
        {
            return IO::write(writer, _capitalized ? "INF" : "inf");
        }

        int64_t ipart = (int64_t)value;
        written += TRY(format(writer, ipart));

        if (_precision > 0)
        {
            written += TRY(IO::write(writer, '.'));

            double fpart = value - (double)ipart;

            for (int i = 0; i < _precision; i++)
            {
                fpart *= _base;
                written += TRY(IO::write(writer, digit((int)fpart % _base)));
            }
        }

        return written;
    }
};
