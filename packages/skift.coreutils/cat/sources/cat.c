/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/filesystem.h>

int cat(const char *path)
{
    int fd = sk_filesystem_open(path, OPENOPT_READ);

    if (fd != 0)
    {
        file_stat_t stat = {0};
        sk_filesystem_fstat(fd, &stat);

        if (stat.type == FSDIRECTORY)
        {
            printf("%s: is a directory\n", path);
            return -1;
        }
        else
        {
            int size;
            byte buffer[1025] = {0};

            while ((size = sk_filesystem_read(fd, &buffer, 1024)) > 0)
            {
                buffer[size] = '\0';
                puts(buffer);
            }
        }

        sk_filesystem_close(fd);
        
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
    if (argc == 2)
    {
        return cat(argv[1]);
    }

    return -1;
}
