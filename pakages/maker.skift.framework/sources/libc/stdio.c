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

#include <stdarg.h>
#include <stdio.h>
#include <skift/generic.h>


// %5d
// %8x
// %10s

/*#define APPEND(c) do { s[si++] = (c); s[si] = 0; if (si == n) return 0; } while(0);
#define PEEK() do { c = fmt[fi++]; if (c == 0) return 0; } while(0)

typedef enum
{
    FSTATE_ESC,
    FSTATE_PARSE,
    FSTATE_PADDING
} formater_state_t;

void vsnprintf(char* s, size_t n, const char * fmt)
{
    char c = '\0';
    uint fi = 0; // fmt index
    uint si = 0; // string index

    formater_state_t state = FSTATE_ESC;

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
                
                break;
            default:
                break;
        }
    }
}*/