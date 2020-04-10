#include <libsystem/utils/NumberParser.h>

static const char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
static const char *digits_capitalized = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

bool parse_uint(NumberParser parser, const char *str, size_t size, unsigned int *result)
{
    if (size == 0)
    {
        *result = 0;
        return true;
    }

    unsigned int value = 0;

    for (size_t i = 0; (i < size && str[i]); i++)
    {
        value = value * parser.base;

        for (int j = 0; j < parser.base; j++)
        {
            if ((digits[j] == str[i]) || (digits_capitalized[j] == str[i]))
            {
                value += j;
            }
        }
    }

    *result = value;
    return true;
}

bool parse_int(NumberParser parser, const char *str, size_t size, int *result)
{
    if (size == 0)
    {
        *result = 0;
        return true;
    }

    bool is_negative = str[0] == '-';
    str++;
    size--;

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

// TODO
// bool parse_double(NumberParser parser, const char *str, size_t size, int *result)
// {
// }