/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/debug.h>
#include <libsystem/process/Process.h>

void debug_hexdump(Stream *stream, char *desc, const void *addr, size_t len)
{
    size_t i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char *)addr;

    stream_printf(stream, "Hexdump from %d at %08x of %d bites\n", process_this(), addr, len);

    // Output description if given.
    if (desc != NULL)
        stream_printf(stream, "%s:\n", desc);

    if (len == 0)
        return;

    // Process every byte in the data.
    for (i = 0; i < len; i++)
    {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0)
        {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                stream_printf(stream, "  %s\n", buff);

            // Output the offset.
            stream_printf(stream, "  %04x ", i);
        }

        // Now the hex code for the specific character.
        stream_printf(stream, " %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
        {
            buff[i % 16] = '.';
        }
        else
        {
            buff[i % 16] = pc[i];
        }

        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0)
    {
        stream_printf(stream, "   ");
        i++;
    }

    // And print the final ASCII bit.
    stream_printf(stream, "  %s\n", buff);
}