#ifndef __KERNEL__
#    include <math.h>
#endif
#include <skift/NumberFormatter.h>
#include <string.h>

static const char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
static const char *digits_capitalized = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

size_t format_uint(NumberFormater formater, unsigned long value, char *str, size_t size)
{
    if (size == 0)
    {
        return 0;
    }

    size_t written = 0;

    str[0] = '\0';
    if (value == 0)
    {
        strnapd(str, '0', size);
        written++;
    }

    while (value != 0)
    {
        if (formater.capitalized)
        {
            strnapd(str, digits[value % formater.base], size);
        }
        else
        {
            strnapd(str, digits_capitalized[value % formater.base], size);
        }

        value /= formater.base;
        written++;
    }

    if (formater.padded_with_zero)
    {
        while (written < size)
        {
            strnapd(str, '0', size);
            written++;
        }
    }

    strrvs(str);

    return written;
}

size_t format_int(NumberFormater formater, long value, char *str, size_t size)
{
    if (size == 0)
    {
        return 0;
    }

    size_t written = 0;
    str[0] = '\0';

    if (value < 0)
    {
        strnapd(str, '-', size);
        str++;
        size--;
        value = -value;
        written++;
    }

    written += format_uint(formater, value, str, size);
    return written;
}

#ifndef __KERNEL__

size_t format_double(NumberFormater formater, double value, char *str, size_t size)
{
    int ipart = (int)value;

    float fpart = value - (float)ipart;

    size_t written = format_int(formater, ipart, str, size);

    if (formater.after_point != 0)
    {
        strnapd(str, '.', size);
        written++;

        fpart = fpart * pow(formater.base, formater.after_point);

        format_int(formater, (int)fpart, str + written, size - written);
    }

    return written;
}

#endif
