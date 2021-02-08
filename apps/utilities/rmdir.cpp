#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/io/Filesystem.h>
#include <libsystem/io_new/Directory.h>
#include <libsystem/io_new/Streams.h>
#include <libutils/Path.h>
#include <stdio.h>

static bool force = false;
static bool remove_parents = false;
static bool verbose = false;

static const char *usages[] = {
    "DIRECTORIES..."
    "OPTIONS... DIRECTORIES...",
    nullptr,
};

static CommandLineOption options[] = {
    COMMANDLINE_OPT_HELP,

    COMMANDLINE_OPT_BOOL("force", 'f', force,
                         "ignore non-existent files and arguments",
                         COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("parents", 'p', remove_parents,
                         "remove DIRECTORY and its ancestors; e.g., 'rmdir -p a/b/c' is similar to 'rmdir a/b/c a/b a'",
                         COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_BOOL("verbose", 'v', verbose,
                         "output a diagnostic for every directory processed",
                         COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_END,
};

static CommandLine cmdline = CMDLINE(
    usages,
    options,
    "Remove the DIRECTORY(ies), if they are empty.",
    "Options can be combined.");

Result rmdir(String path)
{
    System::Directory directory{path};

    if (!force && directory.entries().any())
    {
        return ERR_DIRECTORY_NOT_EMPTY;
    }

    Result unlink_result = filesystem_unlink(path.cstring());

    if (unlink_result != SUCCESS)
    {
        System::errln("rmdir: successfully removed '{}': {}", path, get_result_description(unlink_result));
    }
    else if (verbose)
    {
        System::errln("rmdir: successfully removed '{}': {}", path, get_result_description(unlink_result));
    }

    return unlink_result;
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
            if (rmdir(argv[i]) != SUCCESS)
            {
                return PROCESS_FAILURE;
            }
        }
        else
        {
            auto path = Path::parse(argv[i]);

            while (path.length() > 0)
            {
                auto result = rmdir(path.string().cstring());

                if (result != SUCCESS)
                {
                    return PROCESS_FAILURE;
                }

                path = path.dirpath();
            }
        }
    }

    return PROCESS_SUCCESS;
}
