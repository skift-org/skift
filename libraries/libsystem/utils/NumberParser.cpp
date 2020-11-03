#include <libsystem/core/CString.h>
#include <libsystem/math/Math.h>
#include <libsystem/utils/NumberParser.h>
#include <libutils/Scanner.h>

static constexpr const char *XDIGITS = "0123456789abcdefghijklmnopqrstuvwxyz";
static constexpr const char *XDIGITS_CAPITALIZED = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

unsigned int parse_uint_inline(NumberParser parser, const char *str, uint default_value)
{
    uint result = 0;
    if (parse_uint(parser, str, strlen(str), &result))
    {
        return result;
    }

    return default_value;
}

bool parse_uint(NumberParser parser, const char *str, size_t size, unsigned int *result)
{
    if (str == nullptr || size == 0)
    {
        *result = 0;
        return false;
    }

    unsigned int value = 0;

    for (size_t i = 0; (i < size && str[i]); i++)
    {
        value = value * parser.base;

        for (int j = 0; j < parser.base; j++)
        {
            if ((XDIGITS[j] == str[i]) || (XDIGITS_CAPITALIZED[j] == str[i]))
            {
                value += j;
            }
        }
    }

    *result = value;
    return true;
}

int parse_int_inline(NumberParser parser, const char *str, int default_value)
{
    if (str == nullptr)
    {
        return default_value;
    }

    int result = 0;
    if (parse_int(parser, str, strlen(str), &result))
    {
        return result;
    }

    return default_value;
}

bool parse_int(NumberParser parser, const char *str, size_t size, int *result)
{
    if (str == nullptr || size == 0)
    {
        *result = 0;
        return false;
    }

    bool is_negative = str[0] == '-';
    if (is_negative)
    {
        str++;
        size--;
    }

    unsigned int unsigned_value = 0;
    if (!parse_uint(parser, str, size, &unsigned_value))
    {
        *result = 0;
        return false;
    }

    if (is_negative)
    {
        *result = -unsigned_value;
    }
    else
    {
        *result = unsigned_value;
    }

    return true;
}

#ifndef __KERNEL__

static constexpr const char *DIGITS = "0123456789";

static int digits(Scanner &scan)
{
    int digits = 0;

    while (scan.current_is(DIGITS))
    {
        digits *= 10;
        digits += scan.current() - '0';
        scan.foreward();
    }

    return digits;
}

bool parse_double(NumberParser parser, const char *str, size_t size, double *result)
{
    assert(parser.base == 10);

    StringScanner scan{str, size};

    int ipart_sign = 1;

    if (scan.skip('-'))
    {
        ipart_sign = -1;
    }

    if (scan.skip('+'))
    {
        ipart_sign = +1;
    }

    int ipart = 0;

    if (scan.current_is(DIGITS))
    {
        ipart = digits(scan);
    }

    double fpart = 0;

    if (scan.skip('.'))
    {
        double multiplier = 0.1;

        while (scan.current_is(DIGITS))
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

        int exp_sign = 1;

        if (scan.skip('-'))
        {
            exp_sign = -1;
        }

        if (scan.skip('+'))
        {
            exp_sign = -1;
        }

        exp = digits(scan) * exp_sign;
    }

    *result = ipart_sign * (ipart + fpart) * pow(10, exp);

    return true;
}

#endif
