#include <stdarg.h>

#include <skift/string.h>

/* --- String basic operations ---------------------------------------------- */

uint sk_string_lenght(const char *str)
{
    uint size = 0;

    while (str[size])
    {
        size++;
    }

    return size;
}

char *sk_string_append_char(char *str, char c)
{
    uint lenght = sk_string_lenght(str);

    str[lenght] = c;
    str[lenght + 1] = '\0';

    return str;
}

char *sk_string_append_string(char *dest, const char *str)
{
    uint dest_size = sk_strlen(dest);
    uint src_size = sk_strlen(str);

    char *d = dest;

    for (size_t i = 0; i < src_size; i++)
    {
        d[dest_size + i] = str[i];
    }

    d[dest_size + src_size] = '\0';

    return dest;
}

void sk_string_reverse(char * str)
{
    int c, i, j;
    for (i = 0, j = sk_string_lenght(str) - 1; i < j; i++, j--)
    {
        c = str[i];
        str[i] = str[j];
        str[j] = c;
    }
}

static const char *basechar = "0123456789abcdefghijklmnopqrstuvwxyz";
static const char *basechar_maj = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

uint sk_string_to_uint(const char *str, uchar base)
{
    uint value = 0;
    char c;
    for (uint i = 0; (c = str[i]); i++)
    {
        for (uint j = 0; j < base; j++)
        {
            if ((basechar[j] == str[i]) | (basechar_maj[j] == str[i]))
            {
                value += j;
            }
        }
    }

    return value;
}

void sk_uint_to_string(uint value, char *str, uchar base)
{
    str[0] = '\0';

    if (value == 0)
    {
        sk_strapdc(str, '0');
        return;
    }

    while (value != 0)
    {
        sk_strapdc(str, basechar_maj[value % base]);
        value /= base;
    }

    sk_string_reverse(str);
}

/* --- String format logic -------------------------------------------------- */

int sk_string_format_va(char *str, const char *fmt, va_list va)
{
    str[0] = '\0';

    bool wait_for_format = false;

    for (unsigned int i = 0; fmt[i]; i++)
    {
        char c = fmt[i];

        if (c == '%' && !wait_for_format)
        {
            wait_for_format = true;
        }
        else if (wait_for_format)
        {
            char temp[32];
            switch (c)
            {
            case 'b':
                sk_uint_to_string(va_arg(va, unsigned int), temp, 2);
                sk_strapds(str, temp);
                break;

            case 'i':
            case 'd':
            {
                bool isneg = false;
                int value = va_arg(va, unsigned int);
                unsigned int uvalue = 0;

                if (value < 0)
                {
                    isneg = true;
                    uvalue = 0 - value;
                }
                else
                {
                    uvalue = value;
                }

                if (isneg)
                {
                    sk_strapdc(str, '-');
                }

                sk_uint_to_string(uvalue, temp, 10);
                sk_strapds(str, temp);
            }
            break;
            case 'u':
                sk_uint_to_string(va_arg(va, uint), temp, 10);
                sk_strapds(str, temp);
                break;

            case 'x':
                sk_uint_to_string(va_arg(va, uint), temp, 16);
                sk_strapds(str, temp);
                break;

            case 'c':
                sk_strapdc(str, va_arg(va, char));
                break;

            case 's':
                sk_strapds(str, va_arg(va, const char *));
                break;

            case '%':
                sk_strapdc(str, '%');
                break;

            default:
                sk_strapdc(str, '%');
                sk_strapdc(str, c);
                break;
            }

            wait_for_format = false;
        }
        else
        {
            sk_strapdc(str, c);
        }
    }

    return sk_strlen(str);
}