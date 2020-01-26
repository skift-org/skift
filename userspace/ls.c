/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/cmdline.h>
#include <libsystem/cstring.h>
#include <libsystem/error.h>
#include <libsystem/io/Stream.h>
#include <libsystem/logger.h>

static bool option_all = false;
static bool option_list = false;
static bool option_color = false;

static const char *usages[] = {
    "",
    "FILES...",
    "OPTION... FILES...",
    NULL,
};

static CommandLineOption options[] = {
    COMMANDLINE_OPT_HELP,

    COMMANDLINE_OPT_BOOL("all", 'a', option_all, "Do not ignore entries starting with '.'.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("list", 'l', option_list, "Long listing mode.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("color", 'c', option_color, "Enable colored output.", COMMANDLINE_NO_CALLBACK),

    COMMANDLINE_OPT_END};

static CommandLine cmdline = CMDLINE(
    usages,
    options,
    "List files and directories in the current working directory by default.",
    "Options can be combined.");

const char *file_type_name[] = {
    "-", // None
    "-", // File
    "c", // Char devoce
    "d", // Directory
    "p", // Pipe
};

void ls_print_entry(DirectoryEntry *entry)
{
    FileState *stat = &entry->stat;

    if (option_list)
    {
        printf("%srwxrwxrwx %5d ", file_type_name[stat->type], stat->size);
    }

    if (option_all || entry->name[0] != '.')
        printf((stat->type == FILE_TYPE_DIRECTORY && option_color) ? "\e[1;34m%s\e[0m/ " : "%s  ", entry->name);

    if (option_list)
    {
        printf("\n");
    }
}

int ls(const char *target_path, bool with_prefix)
{
    Stream *directory = stream_open(target_path, OPEN_READ);

    if (handle_has_error(directory))
    {
        handle_printf_error(directory, "ls: cannot access '%s'", target_path);
        stream_close(directory);
        return -1;
    }

    if (with_prefix)
    {
        printf("%s:\n", target_path);
    }

    FileState stat = {0};
    stream_stat(directory, &stat);

    if (stat.type == FILE_TYPE_DIRECTORY)
    {
        DirectoryEntry entry;

        while (stream_read(directory, &entry, sizeof(entry)) > 0)
        {
            ls_print_entry(&entry);
        }
    }
    else
    {
        DirectoryEntry entry;
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

    stream_close(directory);

    return 0;
}

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);

    if (argc == 2)
    {
        return ls(argv[1], false);
    }
    else if (argc > 2)
    {
        for (int i = 1; i < argc; i++)
        {
            ls(argv[i], true);
        }

        return 0;
    }
    else
    {
        return ls(".", false);
    }
}
