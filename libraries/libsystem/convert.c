/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/assert.h>
#include <libsystem/cstring.h>
#include <libsystem/convert.h>

static const char *basechar = "0123456789abcdefghijklmnopqrstuvwxyz";
static const char *basechar_maj = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

uint convert_string_to_uint(const char *str, int n, int base)
{
    assert(str);
    assert(base > 0 && base < 36);

    uint value = 0;

    for (int i = 0; (i < n && str[i]); i++)
    {
        value = value * base;

        for (int j = 0; j < base; j++)
        {
            if ((basechar[j] == str[i]) || (basechar_maj[j] == str[i]))
            {
                value += j;
            }
        }
    }

    return value;
}

int convert_uint_to_string(uint value, char *str, int n, int base)
{
    assert(str);
    assert(base > 0 && base < 36);

    str[0] = '\0';
    if (value == 0)
    {
        strnapd(str, '0', n);
        return 1;
    }

    int written = 0;

    while (value != 0)
    {
        strnapd(str, basechar_maj[value % base], n);
        value /= base;

        written++;
    }

    strrvs(str);

    return written;
}