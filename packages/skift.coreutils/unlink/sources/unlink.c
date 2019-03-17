/* Copyright © 2018-2019 N. Van Bossuyt.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/filesystem.h>

int unlink(const char *path)
{
    return sk_filesystem_unlink(path);
}

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        return unlink(argv[1]);
    }

    return -1;
}
