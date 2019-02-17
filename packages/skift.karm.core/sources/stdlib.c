/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdlib.h>
#include <string.h>

const char * basechar     = "0123456789abcdefghijklmnopqrstuvwxyz";
const char * basechar_maj = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

unsigned int stoi(const char * str, unsigned char base)
{
    unsigned int i = 0;
    unsigned int value = 0;

    while (str[i])
    {
        value = value * base;

        for (unsigned char j = 0; j < base; j++)
        {
            if ((basechar[j] == str[i]) | (basechar_maj[j] == str[i]))
            {
                value += j;
            }
        }

        i++;
    }

    return value;
}

void itos(unsigned int value, char * buffer, unsigned char base)
{
    buffer[0] = '\0';   
    if (value == 0){ strapd(buffer, '0'); return; }
    
    unsigned int v = value;

    while (v != 0)
    {
        strapd(buffer, basechar_maj[v % base]);
        v /= base;
    }

    strrvs(buffer);
}