#include <assert.h>

#include <libio/Streams.h>
#include <libsystem/process/Process.h>
#include <libutils/NumberParser.h>

#include "shell/Shell.h"

int shell_builtin_cd(int argc, const char **argv)
{
    const char *new_directory = "/";

    if (argc == 2)
    {
        new_directory = argv[1];
    }

    Result result = process_set_directory(new_directory);

    if (result != SUCCESS)
    {
        IO::errln("cd: Cannot access '{}': {}", new_directory, result_to_string(result));
        return PROCESS_FAILURE;
    }

    return PROCESS_SUCCESS;
}

int shell_builtin_exit(int argc, const char **argv)
{
    if (argc == 2)
    {
        process_exit(parse_int_inline(PARSER_DECIMAL, argv[1], -1));
    }
    else
    {
        process_exit(PROCESS_SUCCESS);
    }

    ASSERT_NOT_REACHED();
}

int shell_which(int argc, const char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        auto executable = find_command_path(argv[i]);

        if (executable.present())
        {
            IO::outln("{}", executable.unwrap());
        }
        else
        {
            IO::errln("{}: not in POSIX.PATH", argv[i]);
        }
    }

    return PROCESS_SUCCESS;
}

static ShellBuiltin _shell_builtins[] = {
    {"cd", shell_builtin_cd},
    {"exit", shell_builtin_exit},
    {"which", shell_which},
    {nullptr, nullptr},
};

ShellBuiltinCallback shell_get_builtin(const char *name)
{
    for (int i = 0; _shell_builtins[i].name != nullptr; i++)
    {
        if (strcmp(_shell_builtins[i].name, name) == 0)
        {
            return _shell_builtins[i].handler;
        }
    }

    return nullptr;
}
