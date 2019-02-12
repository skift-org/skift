/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/filesystem.h>

int mkdir(const char *path)
{
    return sk_filesystem_mkdir(path);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (argc == 2)
    {
        return mkdir(argv[1]);
    }

    return -1;
}
