#include <libio/Directory.h>
#include <libio/Streams.h>
#include <libsystem/io/Filesystem.h>
#include <libutils/ArgParse.h>

static bool force = false;
static bool remove_parents = false;
static bool verbose = false;

Result rmdir(String path)
{
    IO::Directory directory{path};

    if (!force && directory.entries().any())
    {
        return ERR_DIRECTORY_NOT_EMPTY;
    }

    Result unlink_result = filesystem_unlink(path.cstring());

    if (unlink_result != SUCCESS)
    {
        IO::errln("rmdir: could not remove '{}': {}", path, get_result_description(unlink_result));
    }
    else if (verbose)
    {
        IO::errln("rmdir: successfully removed '{}'", path);
    }

    return unlink_result;
}

int main(int argc, const char *argv[])
{
    ArgParse args;

    args.show_help_if_no_operand_given();

    args.usage("DIRECTORIES...");
    args.usage("OPTIONS... DIRECTORIES...");

    args.prologue("Delete the DIRECTORY or DIRECTORIES, if they are empty.");

    args.option(force, 'f', "force", "ignore non-existent files and arguments");
    args.option(remove_parents, 'p', "parents", "remove DIRECTORY and its ancestors; e.g., 'rmdir -p a/b/c' is similar to 'rmdir a/b/c a/b a'");
    args.option(verbose, 'v', "verbose", "output a diagnostic for every directory processed");

    args.epiloge("Options can be combined.");

    auto parse_result = args.eval(argc, argv);
    if (parse_result != ArgParseResult::SHOULD_CONTINUE)
    {
        return parse_result == ArgParseResult::SHOULD_FINISH ? PROCESS_SUCCESS : PROCESS_FAILURE;
    }

    for (auto directory : args.argv())
    {
        if (!remove_parents)
        {
            if (rmdir(directory) != SUCCESS)
            {
                return PROCESS_FAILURE;
            }
        }
        else
        {
            auto path = IO::Path::parse(directory);

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
