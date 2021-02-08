#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/io/Directory.h>
#include <libsystem/io/Filesystem.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

static bool parent = false;
static bool verbose = false;

static CommandLineOption options[] = {
    COMMANDLINE_OPT_HELP,
    COMMANDLINE_OPT_BOOL("parents", 'p', parent, "Make parent directories as needed.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("verbose", 'v', verbose, "Print a message for each created directory.", COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_END};

static const char *usages[] = {
    "[OPTION]... DIRECTORY...",
    nullptr,
};

static CommandLine cmdline = CMDLINE(
    usages,
    options,
    "Create the DIRECTORY(ies), if they do not already exist.",
    "Options can be combined.");

void print_path(const char *path)
{
    if (verbose)
    {
        printf("mkdir: created directory %s\n", path);
    }
}

void mkdir_parent_dirs(const char *path)
{
    if (directory_exist(path))
    {
        return;
    }

    const char *iter = path;
    size_t path_len = strlen(path);
    char *construct_parent_dirs = (char *)calloc(path_len + 1, sizeof(char));
    char *iter_recursively = construct_parent_dirs;

    if (!path_len)
    {
        free(construct_parent_dirs);
        return;
    }

    while (*iter != '\0')
    {
        *iter_recursively = *iter;

        if (*iter == '/')
        {
            if (!directory_exist(construct_parent_dirs))
            {
                filesystem_mkdir(construct_parent_dirs);
                print_path(construct_parent_dirs);
            }
        }

        ++iter_recursively;
        ++iter;
    }

    --iter;
    if (*iter != '/')
    {
        if (!directory_exist(construct_parent_dirs))
        {
            filesystem_mkdir(construct_parent_dirs);
            print_path(construct_parent_dirs);
        }
    }

    free(construct_parent_dirs);
}

int main(int argc, char **argv)
{
    cmdline_parse(&cmdline, argc, argv);

    if (parent)
    {
        for (int i = 1; i < argc; ++i)
        {
            mkdir_parent_dirs(argv[i]);
        }

        return PROCESS_SUCCESS;
    }

    else
    {
        int result = 0;

        for (int i = 1; i < argc; i++)
        {
            result |= filesystem_mkdir(argv[i]);
            print_path(argv[i]);
        }

        return result;
    }
}
