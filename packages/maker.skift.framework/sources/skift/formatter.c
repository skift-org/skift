/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <skift/ascii.h>

#include <skift/formatter.h>

/* --- Build in formatters --------------------------------------------------- */

int sk_format_binary(printf_info_t* info, va_list* va)
{
    uint v = va_arg(*va, uint);

    char buffer[33] = {0};
    itos(v, buffer, 2);
    
    PADDING(buffer, PFALIGN_RIGHT);

    for(int i = 0; buffer[i]; i++)
        APPEND(buffer[i]);

    PADDING(buffer, PFALIGN_LEFT);
    
    return strlen(info->output);
}

int sk_format_octal(printf_info_t* info, va_list* va)
{
    uint v = va_arg(*va, uint);

    char buffer[12] = {0};
    itos(v, buffer, 8);
    
    PADDING(buffer, PFALIGN_RIGHT);

    for(int i = 0; buffer[i]; i++)
        APPEND(buffer[i]);

    PADDING(buffer, PFALIGN_LEFT);
    
    return strlen(info->output);
}

int sk_format_decimal(printf_info_t* info, va_list* va)
{
    int v = va_arg(*va, int);

    char buffer[13] = {0};

    if (v < 0)
    {
        v = 0 - v;

        buffer[0] = '-';
        buffer[1] = '\0';

        itos(v, buffer + 1, 10);
    }
    else
    {
        itos(v, buffer, 10);
    }

    PADDING(buffer, PFALIGN_RIGHT);

    for(int i = 0; buffer[i]; i++)
        APPEND(buffer[i]);

    PADDING(buffer, PFALIGN_LEFT);
    
    return strlen(info->output);
}

int sk_format_hexadecimal(printf_info_t* info, va_list* va)
{
    uint v = va_arg(*va, uint);

    char buffer[9] = {0};
    itos(v, buffer, 16);
    
    PADDING(buffer, PFALIGN_RIGHT);

    for(int i = 0; buffer[i]; i++)
        APPEND(buffer[i]);

    PADDING(buffer, PFALIGN_LEFT);
    
    return strlen(info->output);
}

int sk_format_char(printf_info_t* info, va_list* va)
{
    char v = va_arg(*va, char);

    char buffer[2] = {v, 0};

    PADDING(buffer, PFALIGN_RIGHT);

    for(int i = 0; buffer[i]; i++)
        APPEND(buffer[i]);

    PADDING(buffer, PFALIGN_LEFT);
    
    return strlen(info->output);
}

int sk_format_string(printf_info_t* info, va_list* va)
{
    const char* v = va_arg(*va, char*);

    PADDING(v, PFALIGN_RIGHT);

    for(int i = 0; v[i]; i++)
        APPEND(v[i]);

    PADDING(v, PFALIGN_LEFT);
    
    return strlen(info->output);
}

/* --- formatters managment -------------------------------------------------- */

formatter_t formatters[52];

void sk_formatter_init()
{
    sk_formatter_register('b', (formatter_t)sk_format_binary);
    sk_formatter_register('o', (formatter_t)sk_format_octal);
    sk_formatter_register('d', (formatter_t)sk_format_decimal);
    sk_formatter_register('x', (formatter_t)sk_format_hexadecimal);
    sk_formatter_register('c', (formatter_t)sk_format_char);
    
    sk_formatter_register('s', (formatter_t)sk_format_string);
}

bool sk_formatter_register(char c, formatter_t formatter)
{
    if (isalpha(c))
    {
        // Calculate the index of the entry.
        if (islower(c))
        {
            c = c - ASCII_a;
        }
        else 
        {
            c = c - ASCII_A + 26;
        }

        formatters[(int)c] = formatter;

        return true;
    }
    else
    {
        return false;
    }
}

int sk_formatter_format(printf_info_t* info, char sel, va_list* va)
{
    if (isalpha(sel))
    {
        if (islower(sel))
        {
            sel = sel - ASCII_a;
        }
        else 
        {
            sel = sel - ASCII_A + 26;
        }

        if (formatters[(int)sel] != NULL)
        {
            return formatters[(int)sel](info, va);
        }
        else
        {
            APPEND('%');
            APPEND(sel);
        }
    }
    else
    {
        APPEND('%');
        APPEND(sel);
    }

    return strlen(info->output);
}