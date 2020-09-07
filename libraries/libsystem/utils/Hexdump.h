#pragma once

#include <ctype.h>
#include <libsystem/io/Stream.h>

static inline void hexdump(const void *ptr, int buflen)
{
    unsigned char *buf = (unsigned char *)ptr;
    for (int i = 0; i < buflen; i += 16)
    {
        printf("%06x: ", i);

        for (int j = 0; j < 16; j++)
        {
            if (i + j < buflen)
            {
                printf("%02x ", buf[i + j]);
            }
            else
            {
                printf("   ");
            }
        }

        printf(" ");

        for (int j = 0; j < 16; j++)
        {
            if (i + j < buflen)
            {
                char c = isprint(buf[i + j]) ? buf[i + j] : '.';

                printf("%c", c);
            }
        }

        printf("\n");
    }
}
