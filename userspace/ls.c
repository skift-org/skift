/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cstring.h>
#include <libsystem/error.h>
#include <libsystem/logger.h>
#include <libsystem/iostream.h>
#include <libsystem/cmdline.h>

static bool option_all = false;
static bool option_list = false;
static bool option_color = false;

static const char *usages[] = {
    "",
    "FILES...",
    "OPTION... FILES...",
    NULL,
};

static cmdline_option_t options[] = {
    CMDLINE_OPT_HELP,

    CMDLINE_OPT_BOOL("all", 'a', option_all, "Do not ignore entries starting with '.'.", CMDLINE_NO_CALLBACK),
    CMDLINE_OPT_BOOL("list", 'l', option_list, "Long listing mode.", CMDLINE_NO_CALLBACK),
    CMDLINE_OPT_BOOL("color", 'c', option_color, "Enable colored output.", CMDLINE_NO_CALLBACK),

    CMDLINE_OPT_END};

static cmdline_t cmdline = CMDLINE(usages, options, "List files and directories in the current working directory by default.", "Options can be combined.");

const char *file_type_name[] = {
    "-", // None
    "-", // File
    "c", // Char devoce
    "d", // Directory
    "p", // Pipe
};

void ls_print_entry(IOStreamDirentry *entry)
{
    IOStreamState *stat = &entry->stat;

    if (option_list)
    {
        printf("%srwxrwxrwx %5d ", file_type_name[stat->type], stat->size);
    }

    if (option_all || entry->name[0] != '.')
        printf((stat->type == IOSTREAM_TYPE_DIRECTORY && option_color) ? "\e[1;34m%s\e[0m/ " : "%s  ", entry->name);

    if (option_list)
    {
        printf("\n");
    }
}

int ls(const char *target_path)
{
    IOStream *dir = iostream_open(target_path, IOSTREAM_READ);

    if (dir != NULL)
    {
        IOStreamState stat = {0};
        iostream_stat(dir, &stat);

        if (stat.type == IOSTREAM_TYPE_DIRECTORY)
        {
            IOStreamDirentry entry;

            while (iostream_read(dir, &entry, sizeof(entry)) > 0)
            {
                ls_print_entry(&entry);
            }
        }
        else
        {
            IOStreamDirentry entry;
            entry.stat = stat;
            Path *p = path(target_path);
            strlcpy(entry.name, path_filename(p), PATH_LENGHT);
            ls_print_entry(&entry);
            path_delete(p);
        }

        if (!option_list)
        {
            printf("\n");
        }

        iostream_close(dir);

        return 0;
    }
    else
    {
        iostream_printf(err_stream, "ls: cannot access '%s'", target_path);
        error_print("");
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
        return ls(".");
    }
}
