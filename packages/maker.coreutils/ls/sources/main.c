/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/filesystem.h>

int ls(const char* path)
{
    int dir = sk_filesystem_open(path, OPENOPT_READ);

    if (dir == 0)
    {
        printf("Failed to open directory '%s'.\n", path);
        return -1;
    }
    else
    {
        directory_entry_t entry;

        while (sk_filesystem_read(dir, &entry, sizeof(entry)) > 0)
        {
            printf("%s  ", entry.name);
        }

        sk_filesystem_close(dir);

        return 0;
    }
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;


    return ls("/bin");
}
