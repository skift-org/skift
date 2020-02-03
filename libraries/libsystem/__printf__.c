/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

// printf.c : printf and snprintf internals

#include <libsystem/ctype.h>
#include <libsystem/cstring.h>
#include <libsystem/convert.h>
#include <libsystem/__printf__.h>

int __printf_formate_binary(printf_info_t *info, va_list *va)
{
    uint v = va_arg(*va, uint);

    char buffer[33] = {0};
    convert_uint_to_string(v, buffer, 33, 2);

    PRINTF_PADDING(buffer, PFALIGN_RIGHT);

    for (int i = 0; buffer[i]; i++)
    {
        PRINTF_APPEND(buffer[i]);
    }

    PRINTF_PADDING(buffer, PFALIGN_LEFT);

    return info->written;
}

int __printf_formate_octal(printf_info_t *info, va_list *va)
{
    uint v = va_arg(*va, uint);

    char buffer[33] = {0};
    convert_uint_to_string(v, buffer, 33, 8);

    PRINTF_PADDING(buffer, PFALIGN_RIGHT);

    for (int i = 0; buffer[i]; i++)
    {
        PRINTF_APPEND(buffer[i]);
    }

    PRINTF_PADDING(buffer, PFALIGN_LEFT);

    return info->written;
}

int __printf_formate_decimal(printf_info_t *info, va_list *va)
{
    int v = va_arg(*va, int);

    char buffer[33] = {0};

    if (v < 0)
    {
        v = 0 - v;

        buffer[0] = '-';
        buffer[1] = '\0';

        convert_uint_to_string(v, buffer + 1, 33, 10);
    }
    else
    {
        convert_uint_to_string(v, buffer, 32, 10);
    }

    PRINTF_PADDING(buffer, PFALIGN_RIGHT);

    for (int i = 0; buffer[i]; i++)
    {
        PRINTF_APPEND(buffer[i]);
    }

    PRINTF_PADDING(buffer, PFALIGN_LEFT);

    return info->written;
}

int __printf_formate_hexadecimal(printf_info_t *info, va_list *va)
{
    uint v = va_arg(*va, uint);

    char buffer[33] = {0};
    convert_uint_to_string(v, buffer, 32, 16);

    PRINTF_PADDING(buffer, PFALIGN_RIGHT);

    for (int i = 0; buffer[i]; i++)
    {
        PRINTF_APPEND(buffer[i]);
    }

    PRINTF_PADDING(buffer, PFALIGN_LEFT);

    return info->written;
}

int __printf_formate_char(printf_info_t *info, va_list *va)
{
    char v = va_arg(*va, int);

    // FIXME: support unicode
    char buffer[2] = {v, 0};

    PRINTF_PADDING(buffer, PFALIGN_RIGHT);

    for (int i = 0; buffer[i]; i++)
    {
        PRINTF_APPEND(buffer[i]);
    }

    PRINTF_PADDING(buffer, PFALIGN_LEFT);

    return info->written;
}

int __printf_formate_string(printf_info_t *info, va_list *va)
{
    const char *v = va_arg(*va, char *);

    if (v == NULL)
    {
        v = "(null)";
    }

    PRINTF_PADDING(v, PFALIGN_RIGHT);

    for (int i = 0; v[i]; i++)
    {
        PRINTF_APPEND(v[i]);
    }

    PRINTF_PADDING(v, PFALIGN_LEFT);

    return info->written;
}

static printf_formatter_t formaters[] = {
    /* Binary        */ {'b', __printf_formate_binary},
    /* Octal         */ {'o', __printf_formate_octal},
    /* Decimal       */ {'d', __printf_formate_decimal},
    /* Decimal       */ {'i', __printf_formate_decimal},
    /* Hexadecimal   */ {'x', __printf_formate_hexadecimal},
    /* Hexadecimal   */ {'p', __printf_formate_hexadecimal},

    /* Float         */ {'f', NULL},

    /* Char          */ {'c', __printf_formate_char},
    /* String        */ {'s', __printf_formate_string},

    /* End of the list */ {'\0', NULL},
};

void __printf_formate(printf_info_t *info, char c, va_list *va)
{
    for (int i = 0; formaters[i].c; i++)
    {
        if (formaters[i].c == c && formaters[i].impl != NULL)
        {
            formaters[i].impl(info, va);
            return;
        }
    }

    // For unknown format string juste put into the output.
    const int trash = va_arg(*va, int);
    __unused(trash);
    info->append(info, '%');
    info->append(info, c);
}

int __printf(printf_info_t *info, va_list va)
{
    info->written = 0;
    info->format_offset = 0;
    info->state = PFSTATE_ESC;
    info->align = PFALIGN_RIGHT;
    info->padding = ' ';
    info->lenght = 0;

    if (info->format == NULL)
    {
        for (int i = 0; "(null)"[i]; i++)
        {
            PRINTF_APPEND("(null)"[i]);
        }
        return 0;
    }

    PRINTF_PEEK();

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
                PRINTF_APPEND(info->c);
            }

            PRINTF_PEEK();
            break;

        case PFSTATE_PARSE:
            if (info->c == '0')
            {
                info->padding = '0';
                PRINTF_PEEK();
            }
            else if (info->c == '-')
            {
                info->align = PFALIGN_LEFT;
                PRINTF_PEEK();
            }
            else if (info->c == '%')
            {
                info->state = PFSTATE_ESC;
                PRINTF_APPEND('%');
                PRINTF_PEEK();
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
                PRINTF_PEEK();
            }
            break;

        case PFSTATE_FORMAT_LENGHT:
            if (isdigit(info->c))
            {
                info->lenght *= 10;
                info->lenght += info->c - '0';

                PRINTF_PEEK();
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
            __printf_formate(info, info->c, &va);

            if (info->written == info->allocated)
            {
                return info->written;
            }
            else
            {
                info->lenght = 0;
                info->state = PFSTATE_ESC;
                info->padding = ' ';
                info->align = PFALIGN_RIGHT;

                PRINTF_PEEK();
            }
            break;

        default:
            break;
        }
    }
}