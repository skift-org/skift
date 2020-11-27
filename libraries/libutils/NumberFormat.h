#pragma once

#include <libsystem/math/Math.h>
#include <libutils/StringBuilder.h>
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

    void format(StringBuilder &builder, unsigned int value)
    {
        if (value == 0)
        {
            builder.append('0');
            return;
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

        builder.append(buffer, i);
    }

    void format(StringBuilder &builder, int value)
    {
        if (value < 0)
        {
            builder.append('-');
            value = -value;
        }

        format(builder, (unsigned int)value);
    }

    void format(StringBuilder &builder, float value)
    {
        format(builder, (double)value);
    }

    void format(StringBuilder &builder, double value)
    {
        int ipart = (int)value;
        float fpart = value - (float)ipart;

        format(builder, ipart);

        if (_precision > 0)
        {
            builder.append('.');

            int ifpart = fpart * pow(_base, _precision);
            format(builder, ifpart);
        }
    }
};
