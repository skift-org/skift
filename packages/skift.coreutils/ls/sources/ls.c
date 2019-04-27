/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/logger.h>
#include <skift/iostream.h>
#include <skift/cmdline.h>

static bool option_all = false;
static bool option_list = false;
static bool option_color = false;

static const char *usages[] = {
    "",
    "FILES...",
    "OPTION... FILES...",
    NULL};

static cmdline_option_t options[] = {
    CMDLINE_OPT_HELP,

    CMDLINE_OPT_BOOL("all", 'a', option_all, "Do not ignore entries starting with '.'.", CMDLINE_NO_CALLBACK),
    CMDLINE_OPT_BOOL("list", 'l', option_list, "Long listing mode.", CMDLINE_NO_CALLBACK),
    CMDLINE_OPT_BOOL("color", 'c', option_color, "Enable colored output.", CMDLINE_NO_CALLBACK),

    CMDLINE_OPT_END};

static cmdline_t cmdline = CMDLINE(usages, options, "List files and directories in the current working directory by default.", "Options can be combined.");

const char *file_type_name[] = {
    "file ",
    "dev  ",
    "dir  ",
    "fifo ",
};

void ls_print_entry(iostream_direntry_t *entry)
{
    if (option_list)
    {
        printf(file_type_name[entry->type]);
    }

    if (option_all || entry->name[0] != '.')
        printf((entry->type == IOSTREAM_TYPE_DIRECTORY && option_color) ? "\e[1m%s  \e[0m" : "%s  ", entry->name);

    if (option_list)
    {
        printf("\n");
    }
}

int ls(const char *path)
{
    iostream_t* dir = iostream_open(path, IOSTREAM_READ);

    if (dir != NULL)
    {
        iostream_stat_t state = {0};
        iostream_fstat(dir, &state);

        if (state.type == IOSTREAM_TYPE_DIRECTORY)
        {
            iostream_direntry_t entry;

            while (iostream_read(dir, &entry, sizeof(entry)) > 0)
            {
                ls_print_entry(&entry);
            }
        }
        else
        {
            sk_log(LOG_DEBUG,"This was not a directory");
            printf(path);
        }

        printf("\n");

        iostream_close(dir);

        return 0;
    }
    else
    {
        printf("%s: no such file or directory\n", path);
        return -1;
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
