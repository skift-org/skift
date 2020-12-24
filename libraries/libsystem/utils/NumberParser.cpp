#include <libsystem/core/CString.h>
#include <libsystem/utils/NumberParser.h>
#include <libutils/Scanner.h>
#include <libutils/Strings.h>

static constexpr const char *XDIGITS = "0123456789abcdefghijklmnopqrstuvwxyz";
static constexpr const char *XDIGITS_CAPITALIZED = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

unsigned int parse_uint_inline(NumberParser parser, const char *str, unsigned int default_value)
{
    unsigned int result = 0;

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
            if ((Strings::LOWERCASE_XDIGITS[j] == str[i]) ||
                (Strings::UPPERCASE_XDIGITS[j] == str[i]))
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
