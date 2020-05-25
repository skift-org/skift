
/* scanf.c: scanf and sscanf internals                                        */

/* #include <libsystem/CType.h>
#include <libsystem/CString.h>
#include <libsystem/__scanf__.h>

int __scanf_scan_binary(scanf_info_t *info, va_list *va)
{
}

int __scanf_scan_octal(scanf_info_t *info, va_list *va)
{
}

int __scanf_scan_decimal(scanf_info_t *info, va_list *va)
{
}

int __scanf_scan_hexadecimal(scanf_info_t *info, va_list *va)
{
}

int __scanf_scan_char(scanf_info_t *info, va_list *va)
{
}

int __scanf_scan_string(scanf_info_t *info, va_list *va)
{
}

static scanf_scanner_t formaters[] = {
    {'b', __scanf_scan_binary},
    {'o', __scanf_scan_octal},
    {'d', __scanf_scan_decimal},
    {'x', __scanf_scan_hexadecimal},

    {'f', NULL}, // foat no supported

    {'c', __scanf_scan_char},
    {'s', __scanf_scan_string},

    {'\0', NULL},
};

void __scanf_scan(scanf_info_t *info, char c, va_list *va)
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

int __scanf(scanf_info_t *info, va_list va)
{
    info->n = 0;
    info->format_offset = 0;
    info->state = SFSTATE_ESC;
    info->length = 0;

    if (info->format == NULL)
    {
        for (int i = 0; "(null)"[i]; i++)
        {
            SCANF_APPEND("(null)"[i]);
        }
        return 0;
    }

    SCANF_PEEK();

    while (1)
    {
        switch (info->state)
        {
        case SFSTATE_ESC:
            if (info->c == '%')
            {
                info->state = SFSTATE_PARSE;
            }
            else
            {
                SCANF_APPEND(info->c);
            }

            SCANF_PEEK();
            break;

        case SFSTATE_PARSE:
            if (info->c == '0')
            {
                info->padding = '0';
                SCANF_PEEK();
            }
            else if (info->c == '-')
            {
                info->align = PFALIGN_LEFT;
                SCANF_PEEK();
            }
            else if (info->c == '%')
            {
                info->state = SFSTATE_ESC;
                SCANF_APPEND('%');
                SCANF_PEEK();
            }
            else if (isdigit(info->c))
            {
                info->state = SFSTATE_FORMAT_LENGTH;
            }
            else if (isalpha(info->c))
            {
                info->state = SFSTATE_FINALIZE;
            }
            else
            {
                SCANF_PEEK();
            }
            break;

        case SFSTATE_FORMAT_LENGTH:
            if (isdigit(info->c))
            {
                info->length *= 10;
                info->length += info->c - '0';

                SCANF_PEEK();
            }
            else if (isalpha(info->c))
            {
                info->state = SFSTATE_PARSE;
            }
            else
            {
                info->state = SFSTATE_ESC;
            }
            break;

        case SFSTATE_FINALIZE:
            __scanf_scan(info, info->c, &va);

            if (info->n == info->allocated)
            {
                return info->n;
            }
            else
            {
                info->length = 0;
                info->state = SFSTATE_ESC;
                info->padding = ' ';
                info->align = PFALIGN_RIGHT;

                SCANF_PEEK();
            }
            break;

        default:
            break;
        }
    }
}*/
