#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <skift/ascii.h>

#include <skift/formater.h>

/* --- Build in formaters --------------------------------------------------- */

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

/* --- Formaters managment -------------------------------------------------- */

formater_t formaters[52];

void sk_formater_init()
{
    sk_formater_register('b', (formater_t)sk_format_binary);
    sk_formater_register('o', (formater_t)sk_format_octal);
    sk_formater_register('d', (formater_t)sk_format_decimal);
    sk_formater_register('x', (formater_t)sk_format_hexadecimal);
    sk_formater_register('c', (formater_t)sk_format_char);
    
    sk_formater_register('s', (formater_t)sk_format_string);
}

bool sk_formater_register(char c, formater_t formater)
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

        formaters[(int)c] = formater;

        return true;
    }
    else
    {
        return false;
    }
}

int sk_formater_format(printf_info_t* info, char sel, va_list* va)
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

        if (formaters[(int)sel] != NULL)
        {
            return formaters[(int)sel](info, va);
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