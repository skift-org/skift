
#include <libsystem/Logger.h>
#include <libsystem/Result.h>
#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/io/Stream.h>
#include <libsystem/io_new/Directory.h>
#include <libsystem/io_new/Streams.h>

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

void ls_print_entry(System::Directory::Entry &entry)
{
    FileState stat = entry.stat;

    if (option_list)
    {
        System::format(System::out(), "{}rwxrwxrwx {5} ", file_type_name[stat.type], stat.size);
    }

    if (option_all || entry.name[0] != '.')
    {
        System::format(System::out(), (stat.type == FILE_TYPE_DIRECTORY && option_color) ? "\e[1;34m{}\e[0m/ " : "{}  ", entry.name);
    }

    if (option_list)
    {
        System::out().write("\n");
    }
}

Result ls(const char *target_path, bool with_prefix)
{
    System::Directory directory{target_path};

    if (!directory.exist())
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    if (with_prefix)
    {
        System::outln("{}:", target_path);
    }

    for (auto entry : directory.entries())
    {
        ls_print_entry(entry);
    }

    if (!option_list)
    {
        System::out("\n");
    }

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
