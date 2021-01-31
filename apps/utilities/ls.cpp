
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Directory.h>
#include <libsystem/io/Stream.h>

static bool option_all = false;
static bool option_list = false;
static bool option_color = false;

static const char *usages[] = {
    "",
    "FILES...",
    "OPTION... FILES...",
    nullptr,
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
    "c", // Char device
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
    {
        printf((stat->type == FILE_TYPE_DIRECTORY && option_color) ? "\e[1;34m%s\e[0m/ " : "%s  ", entry->name);
    }

    if (option_list)
    {
        printf("\n");
    }
}

Result ls(const char *target_path, bool with_prefix)
{
    Directory *directory = directory_open(target_path, OPEN_READ);

    if (handle_has_error(directory))
    {
        directory_close(directory);
        return handle_get_error(directory);
    }

    if (with_prefix)
    {
        printf("%s:\n", target_path);
    }

    DirectoryEntry entry;

    while (directory_read(directory, &entry) > 0)
    {
        ls_print_entry(&entry);
    }

    if (!option_list)
    {
        printf("\n");
    }

    directory_close(directory);

    return SUCCESS;
}

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);

    if (argc == 1)
    {
        return ls(".", false);
    }

    Result result;
    int exit_code = PROCESS_SUCCESS;

    for (int i = 1; i < argc; i++)
    {
        result = ls(argv[i], argc > 2);

        if (result != SUCCESS)
        {
            exit_code = PROCESS_FAILURE;
        }
    }

    return exit_code;
}
