/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/Result.h>
#include <libsystem/io/Stream.h>

int cat(const char *path)
{
    Stream *stream = stream_open(path, OPEN_READ);

    if (handle_has_error(stream))
    {
        handle_printf_error(stream, "cat: Cannot access %s", path);
        stream_close(stream);
        return -1;
    }

    FileState stat = {0};
    stream_stat(stream, &stat);

    size_t readed;
    byte buffer[1024];

    while ((readed = stream_read(stream, &buffer, 1024)) != 0)
    {
        if (handle_has_error(stream))
        {
            handle_printf_error(stream, "cat: Failled to read from %s", path);
            stream_close(stream);

            return -1;
        }

        stream_write(out_stream, buffer, readed);

        if (handle_has_error(out_stream))
        {
            handle_printf_error(out_stream, "cat: Failled to write to stdout");
            stream_close(stream);

            return -1;
        }
    }

    stream_close(stream);
    stream_flush(out_stream);

    return 0;
}

int main(int argc, char **argv)
{
    if (argc >= 2)
    {
        int result = 0;

        for (int i = 1; i < argc; i++)
        {
            result |= cat(argv[i]);
        }

        return result;
    }

    return -1;
}
