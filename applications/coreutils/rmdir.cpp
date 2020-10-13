#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/io/Directory.h>
#include <libsystem/io/Filesystem.h>
#include <libsystem/io/Path.h>
#include <libsystem/io/Stream.h>

static bool ignore_fail_on_non_empty = false,
            remove_parents = false,
            verbose = false;

static const char *usages[] = {
    "DIRECTORIES..."
    "OPTIONS... DIRECTORIES...",
    nullptr,
};

static CommandLineOption options[] = {
    COMMANDLINE_OPT_HELP,

    COMMANDLINE_OPT_BOOL("ignore-fail-on-non-empty", 'i', ignore_fail_on_non_empty,
                         "ignore each failure that is solely because a directory is non-empty",
                         COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("parents", 'p', remove_parents,
                         "remove DIRECTORY and its ancestors; e.g., 'rmdir -p a/b/c' is similar to 'rmdir a/b/c a/b a'",
                         COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("verbose", 'v', verbose,
                         "output a diagnostic for every directory processed",
                         COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_END};

static CommandLine cmdline = CMDLINE(
    usages,
    options,
    "Remove the DIRECTORY(ies), if they are empty.",
    "Options can be combined.");

Result rmdir(const char *path)
{
    if (!directory_exist(path))
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    Directory *directory = directory_open(path, OPEN_READ);

    if (handle_has_error(directory))
    {
        directory_close(directory);
        return ERR_NOT_READABLE;
    }

    DirectoryEntry entry;
    if (directory_read(directory, &entry) > 0 && !ignore_fail_on_non_empty)
    {
        directory_close(directory);
        return ERR_DIRECTORY_NOT_EMPTY;
    }

    directory_close(directory);
    Result r = filesystem_unlink(path);

    if (verbose)
    {
        if (result_is_error(r))
        {
            printf("rmdir: failed to remove '%s': %s\n", path, get_result_description(r));
        }
        else
        {
            printf("rmdir: successfully removed '%s': %s\n", path, get_result_description(r));
        }
    }
    return r;
}

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);

    if (argc == 1)
    {
        printf("rmdir: missing operand\nTry 'rmdir --help' for more information.\n");
        return PROCESS_FAILURE;
    }

    for (int i = 1; i < argc; i++)
    {
        if (!remove_parents)
        {
            Result result = rmdir(argv[i]);

            if (result_is_error(result))
            {
                printf("rmdir: failed to remove '%s': %s\n", argv[i], get_result_description(result));
            }
        }
        else
        {
            Path *p = path_create(argv[i]);
            size_t children_count = path_element_count(p);

            for (size_t j = 0; j < children_count; j++)
            {
                Result result = rmdir(path_as_string(p));
                path_pop(p);

                if (result_is_error(result))
                {
                    printf("rmdir: failed to remove '%s': %s\n", argv[i], get_result_description(result));
                }
            }

            path_destroy(p);
        }
    }

    return PROCESS_SUCCESS;
}
