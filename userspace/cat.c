/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/error.h>
#include <libsystem/io/Stream.h>

int cat(const char *path)
{
    Stream *stream = stream_open(path, OPEN_READ);

    if (stream != NULL)
    {
        FileState stat = {0};
        stream_stat(stream, &stat);

        if (stat.type == FILE_TYPE_DIRECTORY)
        {
            printf("%s: is a directory\n", path);
            return -1;
        }
        else
        {
            int size;
            byte buffer[1024];

            while ((size = stream_read(stream, &buffer, 1024)) > 0)
            {
                stream_write(out_stream, buffer, size);
            }
        }

        stream_close(stream);
        stream_flush(out_stream);

        return 0;
    }
    else
    {
        stream_printf(err_stream, "cat: cannot access '%s'", path);
        error_print("");
        return -1;
    }
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
