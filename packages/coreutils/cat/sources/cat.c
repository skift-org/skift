/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/filesystem.h>

int cat(const char* path)
{
    int fd = sk_filesystem_open(path, OPENOPT_READ);

    int size;
    byte buffer[1025];
    buffer[1024] = '\0';

    while ((size = sk_filesystem_read(fd, &buffer, 1024)) > 0)
    {
        buffer[size] = '\0';
        puts(buffer);
    }

    sk_filesystem_close(fd);

    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        return cat(argv[1]);
    }

    return -1;
}
