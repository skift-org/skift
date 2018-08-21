#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lib/ansi.h"

// http://www.termsys.demon.co.uk/vtansi.htm
// http://ascii-table.com/ansi-escape-sequences.php

int ansi_len(const char *esc)
{
    int i = 0;

    if (esc[0] == ANSI_ESC && esc[1] == '[')
    {
        i += 2;

        while (esc[i] == ';' || isdigit(esc[i]))
            i++;

        if (isalpha(esc[i]))
        {
            i++;
        }
    }

    return i;
}

ansitype_t ansi_get_type(const char *esc)
{
    if (esc[0] == ANSI_ESC && esc[1] == '[')
    {
        esc += 2;

        int i = 0;
        while (isdigit(esc[i]) || esc[i] == ';')
        {
            i++;
        }

        char c = esc[i];

        switch (c)
        {
        case 'f':
        case 'H':
            return ANSI_CURSOR_SET;

        case 'A':
            return ANSI_CURSOR_UP;

        case 'B':
            return ANSI_CURSOR_DOWN;

        case 'C':
            return ANSI_CURSOR_FORWARD;

        case 'D':
            return ANSI_CURSOR_BACKWARD;

        case 's':
            return ANSI_CURSOR_SAVE;

        case 'u':
            return ANSI_CURSOR_RESTOR;

        case 'J':
            return ANSI_CLEAR;

        case 'K':
            return ANSI_CLEAR_LINE;

        case 'm':
            return ANSI_SET_MODE;

        default:
            return ANSI_UNKNOW;
        }
    }

    return ANSI_UNKNOW;
}

int ansi_value_count(const char *esc)
{
    int count = 0;
    if (esc[0] == ANSI_ESC && esc[1] == '[')
    {
        int wait_for_digit = 1;

        for (size_t i = 2; isdigit(esc[i]) || esc[i] == ';'; i++)
        {
            if (wait_for_digit && isdigit(esc[i]))
            {
                wait_for_digit = 0;
                count++;
            }

            if (esc[i] == ';')
            {
                wait_for_digit = 1;
            }
        }
    }

    return count;
}

int ansi_value(const char *esc, int index)
{
    char buffer[12];
    buffer[0] = '\0';

    if (esc[0] == ANSI_ESC && esc[1] == '[')
    {
        esc += 2;

        for (size_t i = 0; isdigit(esc[i]) || esc[i] == ';'; i++)
        {
            char c = esc[i];

            if (c == ';')
            {
                index--;
            }
            else if (index == 0)
            {
                strapd(buffer, c);
            }
        }

        return stoi(buffer, 10);
    }

    return 0;
}
