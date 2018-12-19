#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <skift/ascii.h>

#include <skift/formater.h>

/* --- Build in formaters --------------------------------------------------- */

int sk_format_binary(printf_info_t* info, uint v)
{
    char buffer[33];
    itos(v, buffer, 2);
    
    if (info->align == PFALIGN_RIGHT && strlen(buffer) < info->lenght)
    {
        for(int i = 0; i < info->lenght - strlen(buffer); i++)
        {
            APPEND(info->padding);
        }   
    }

    for(int i = 0; buffer[i]; i++)
        APPEND(buffer[i]);

    if (info->align == PFALIGN_LEFT && strlen(buffer) < info->lenght)
    {
        for(int i = 0; i < info->lenght - strlen(buffer); i++)
        {
            APPEND(info->padding);
        }   
    }
    
    return strlen(info->output);
}

int sk_format_octal(printf_info_t* info, uint v)
{
    
}

int sk_format_decimal(printf_info_t* info, int v)
{

}

int sk_format_hexadecimal(printf_info_t* info, uint v)
{

}

int sk_format_hexadecimal_maj(printf_info_t* info, uint v)
{

}

int sk_format_string(printf_info_t* info, void* v)
{

}

/* --- Formaters managment -------------------------------------------------- */

formater_t formater[52];

bool sk_formater_register(char c, formater_t* formater)
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

        formater[c] = formater;

        return true;
    }
    else
    {
        return false;
    }
}

int sk_formater_format(printf_info_t* info, char sel, void* v)
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

        if (formater[sel] != NULL)
        {
            return formater[sel](info, v);
        }
    }
    else
    {
        APPEND('%');
        APPEND(sel);
    }

    return strlen(info->output);
}