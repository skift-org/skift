/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

/* stdio.c: the skift stdio implementation                                    */

/*
 * TODO:
 * - vsnprintf
 *  - binary (%b)
 *  - decimal (%d)
 *  - hexadecimal (%x)
 * 
 *  - char (%c)
 *  - string (%s)
 * 
 *  - Right/Left (%10d %-10d)
 *  - 0 for padding (%010d)
 * 
 *  - custome formater
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <skift/generic.h>


// %5d
// %8x
// %10s

#define APPEND(c) do { s[si++] = (c); s[si] = 0; if (si == n) return 0; } while(0);
#define PEEK() do { c = fmt[fi++]; if (c == 0) return 0; } while(0)

typedef enum
{
    FSTATE_ESC,
    FSTATE_PARSE,
    FSTATE_PADDING,
    FSTATE_FORMAT_LENGHT,
    FSTATE_FORMAT_SELECTOR
} formater_state_t;

typedef char* (*formater_t)(void * data);

void vsnprintf(char* s, size_t n, const char * fmt, va_list va)
{
    char c = '\0';
    uint fi = 0; // fmt index
    uint si = 0; // string index

    formater_state_t state = FSTATE_ESC;
    bool left = false;
    char padding = ' ';
    int lenght = -1;

    while(1)
    {
        switch (state)
        {
            case FSTATE_ESC:
                if (c == '%')
                {
                    state = FSTATE_PARSE;
                }
                else
                {
                   APPEND(c);
                }

                PEEK();
                break;

            case FSTATE_PARSE:
                if (c == '0')
                {
                    padding = '0';
                    PEEK();
                }
                else if (c == '-')
                {
                    left = true;
                    PEEK();
                }
                else if (isdigit(c))
                {
                    state = FSTATE_FORMAT_LENGHT;
                }
                else if (isalpha(c))
                {
                    state = FSTATE_FORMAT_SELECTOR;
                }
                else
                {
                    PEEK();
                }
                break;

            case FSTATE_FORMAT_SELECTOR:
                break;
            
            case FSTATE_FORMAT_LENGHT:
                break;

            default:
                break;
        }
    }
}