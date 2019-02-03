/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/filesystem.h>

int rm(const char *path)
{
    return sk_filesystem_rm(path);
}

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        return rm(argv[1]);
    }

    return -1;
}
