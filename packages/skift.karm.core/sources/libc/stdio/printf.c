/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <skift/formatter.h>

int printf(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    int result = vprintf(fmt, va);

    va_end(va);

    return result;
}

int vprintf(const char *fmt, va_list va)
{
    char buffer[1024];
    int result = vsnprintf(buffer, 1024, fmt, va);
    puts(buffer);

    return result;
}

int sprintf(char *s, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    int result = vsnprintf(s, 0xFFFFFFFF, fmt, va);

    va_end(va);

    return result;
}

int vsprintf(char *s, const char *fmt, va_list va)
{
    return vsnprintf(s, 0xFFFFFFFF, fmt, va);
}

int snprintf(char *s, int n, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    int result = vsnprintf(s, n, fmt, va);

    va_end(va);

    return result;
}

int vsnprintf(char *s, size_t n, const char *fmt, va_list va)
{
    printf_info_t _info =
        {
            .output = s,
            .output_size = n,
            .output_offset = 0,

            .format = fmt,
            .format_offset = 0,

            .state = PFSTATE_ESC,
            .align = PFALIGN_RIGHT,
            .padding = '0',
        };

    printf_info_t *info = &_info;

    PEEK();

    while (1)
    {
        switch (info->state)
        {
        case PFSTATE_ESC:
            if (info->c == '%')
            {
                info->state = PFSTATE_PARSE;
            }
            else
            {
                APPEND(info->c);
            }

            PEEK();
            break;

        case PFSTATE_PARSE:
            if (info->c == '0')
            {
                info->padding = '0';
                PEEK();
            }
            else if (info->c == '-')
            {
                info->align = PFALIGN_LEFT;
                PEEK();
            }
            else if (isdigit(info->c))
            {
                info->state = PFSTATE_FORMAT_LENGHT;
            }
            else if (isalpha(info->c))
            {
                info->state = PFSTATE_FINALIZE;
            }
            else
            {
                PEEK();
            }
            break;

        case PFSTATE_FORMAT_LENGHT:
            if (isdigit(info->c))
            {
                info->lenght *= 10;
                info->lenght += info->c - '0';

                PEEK();
            }
            else if (isalpha(info->c))
            {
                info->state = PFSTATE_PARSE;
            }
            else
            {
                info->state = PFSTATE_ESC;
            }
            break;

        case PFSTATE_FINALIZE:
            sk_formatter_format(info, info->c, &va);

            info->lenght = 0;
            info->state = PFSTATE_ESC;
            info->padding = ' ';
            info->align = PFALIGN_RIGHT;

            PEEK();
            break;

        default:
            break;
        }
    }
}
