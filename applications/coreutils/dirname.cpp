#include <libsystem/cmdline/CMDLine.h>
#include <libsystem/io/Stream.h>
#include <libutils/Path.h>

static bool zero = false;

static const char *usages[] = {
    "NAME...",
    "[OPTION] NAME...",
    nullptr,
};

static CommandLineOption options[] = {
    COMMANDLINE_OPT_HELP,

    COMMANDLINE_OPT_BOOL("zero", 'z', zero,
                         "end each output line with NUL, not newline",
                         COMMANDLINE_NO_CALLBACK),
    COMMANDLINE_OPT_END};

static CommandLine cmdline = CMDLINE(
    usages,
    options,
    "Output each NAME with its last non-slash component and trailing slashes removed;",
    "if NAME contains no  /'s,  output  '.'  (meaning  the  current directory).");

int main(int argc, char **argv)
{
    argc = cmdline_parse(&cmdline, argc, argv);

    if (argc == 1)
    {
        printf("dirname: missing operand\nTry 'dirname --help' for more information.\n");

        return PROCESS_FAILURE;
    }

    for (int i = 1; i < argc; i++)
    {
        char terminator = zero ? '\0' : '\n';

        auto path = Path::parse(argv[i]);
        printf("%s%c", path.dirname().cstring(), terminator);
    }

    return PROCESS_SUCCESS;
}
