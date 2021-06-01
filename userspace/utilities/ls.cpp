#include <libio/Directory.h>
#include <libio/Streams.h>
#include <libshell/ArgParse.h>

static bool option_all = false;
static bool option_list = false;
static bool option_color = false;

const char *file_type_name[] = {
    "-", // None
    "-", // File
    "c", // Char device
    "d", // Directory
    "p", // Pipe
};

void ls_print_entry(IO::Directory::Entry &entry)
{
    HjStat stat = entry.stat;

    if (option_list)
    {
        IO::format(IO::out(), "{}rwxrwxrwx {5} ", file_type_name[stat.type], stat.size);
    }

    if (option_all || entry.name[0] != '.')
    {
        IO::format(IO::out(), (stat.type == HJ_FILE_TYPE_DIRECTORY && option_color) ? "\e[1;34m{}\e[0m/ " : "{}  ", entry.name);
    }

    if (option_list)
    {
        IO::write(IO::out(), "\n");
    }
}

HjResult ls(String target_path, bool with_prefix)
{
    IO::Directory directory{target_path};

    if (!directory.exist())
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (with_prefix)
    {
        IO::outln("{}:", target_path);
    }

    for (auto entry : directory.entries())
    {
        ls_print_entry(entry);
    }

    if (!option_list)
    {
        IO::out("\n");
    }

    return SUCCESS;
}

int main(int argc, const char *argv[])
{
    Shell::ArgParse args;

    args.usage("");
    args.usage("FILES...");
    args.usage("OPTIONS... FILES...");

    args.prologue("List files and directories in the current working directory by default.");

    args.option(option_all, 'a', "all", "Do not ignore entries starting with '.'.");
    args.option(option_list, 'l', "list", "Long listing mode.");
    args.option(option_color, 'c', "color", "Enable colored output.");

    args.prologue("Options can be combined.");

    auto parse_result = args.eval(argc, argv);
    if (parse_result != Shell::ArgParseResult::SHOULD_CONTINUE)
    {
        return parse_result == Shell::ArgParseResult::SHOULD_FINISH
                   ? PROCESS_SUCCESS
                   : PROCESS_FAILURE;
    }

    if (args.argc() == 0)
    {
        return ls(".", false);
    }

    HjResult result;
    int exit_code = PROCESS_SUCCESS;

    for (auto file : args.argv())
    {
        result = ls(file, args.argc() > 2);

        if (result != SUCCESS)
        {
            exit_code = PROCESS_FAILURE;
        }
    }

    return exit_code;
}
