/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <stdio.h>
#include <skift/iostream.h>

int cat(const char *path)
{
    iostream_t *stream = iostream_open(path, IOSTREAM_READ);

    if (stream != NULL)
    {
        iostream_stat_t stat = {0};
        iostream_fstat(stream, &stat);

        if (stat.type == FILE_DIRECTORY)
        {
            printf("%s: is a directory\n", path);
            return -1;
        }
        else
        {
            int size;
            byte buffer[1025] = {0};

            while ((size = iostream_read(stream, &buffer, 1024)) > 0)
            {
                buffer[size] = '\0';
                puts(buffer);
            }
        }

        iostream_close(stream);

        return 0;
    }
    else
    {
        printf("%s: no such file\n", path);
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
