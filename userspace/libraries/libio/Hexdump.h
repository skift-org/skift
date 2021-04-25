#pragma once

#include <ctype.h>

#include <libio/Format.h>

namespace IO
{

static inline ResultOr<size_t> hexdump(Reader &in, Writer &out)
{
    uint8_t buf[16];
    size_t buflen = 0;
    size_t offset = 0;
    size_t written = 0;

    while ((buflen = TRY(in.read(buf, 16))) > 0)
    {
        written += TRY(IO::format(out, "{06x} ", offset));
        offset += buflen;

        for (size_t j = 0; j < 16; j++)
        {
            if (j < buflen)
            {
                written += TRY(IO::format(out, "{02x} ", buf[j]));
            }
            else
            {
                written += TRY(IO::write(out, "   "));
            }
        }

        written += TRY(IO::write(out, " "));

        for (size_t j = 0; j < 16; j++)
        {
            if (j < buflen)
            {
                uint8_t c = isprint(buf[j]) ? buf[j] : '.';
                written += TRY(IO::write(out, c));
            }
        }

        written += TRY(IO::write(out, '\n'));
    }

    return written;
}

} // namespace IO
