#pragma once

#include <ctype.h>
#include <libsystem/io/Stream.h>

static inline void hexdump(const void *ptr, int buflen)
{
    unsigned char *buf = (unsigned char *)ptr;
    for (int i = 0; i < buflen; i += 16)
    {
        stream_format(out_stream, "%06x: ", i);

        for (int j = 0; j < 16; j++)
        {
            if (i + j < buflen)
            {
                stream_format(out_stream, "%02x ", buf[i + j]);
            }
            else
            {
                stream_format(out_stream, "   ");
            }
        }

        stream_format(out_stream, " ");

        for (int j = 0; j < 16; j++)
        {
            if (i + j < buflen)
            {
                char c = isprint(buf[i + j]) ? buf[i + j] : '.';

                stream_format(out_stream, "%c", c);
            }
        }

        stream_format(out_stream, "\n");
    }
}
