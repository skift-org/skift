/* Copyright © 2018-2019 MAKER.                                               */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/filesystem.h>
#include <skift/cmdline.h>

static bool option_all = false;

static const char *usages[] = {
    "ls",
    "ls FILES...",
    "ls OPTION... FILES...",
    NULL};

static cmdline_option_t options[] = {
    CMDLINE_OPT_HELP,
    {.type = CMDLINE_BOOLEAN, .long_name = "all", .short_name = 'a', .value = &option_all},
    CMDLINE_OPT_END};

static cmdline_t cmdline = CMDLINE(usages, options, "List all file and directory.", "The skiftOS project.");

int ls(const char *path)
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
            if (option_all || entry.name[0] != '.')
                printf("%s  ", entry.name);
        }

        printf("\n");
        sk_filesystem_close(dir);

        return 0;
    }
}

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);

    if (argc == 2)
    {
        return ls(argv[1]);
    }
    else if (argc > 2)
    {
        for (int i = 1; i < argc; i++)
        {
            printf("%s:\n", argv[i]);
            ls(argv[i]);
        }

        return 0;
    }
    else
    {
        return ls("/");
    }
}
