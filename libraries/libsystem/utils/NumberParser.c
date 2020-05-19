#include <libsystem/cstring.h>
#include <libsystem/utils/NumberParser.h>

static const char *digits = "0123456789abcdefghijklmnopqrstuvwxyz";
static const char *digits_capitalized = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

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
    if (str == NULL || size == 0)
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
            if ((digits[j] == str[i]) || (digits_capitalized[j] == str[i]))
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
    if (str == NULL)
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
    if (str == NULL || size == 0)
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

// TODO
// bool parse_double(NumberParser parser, const char *str, size_t size, int *result)
// {
// }