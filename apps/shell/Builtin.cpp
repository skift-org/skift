#include <libsystem/Assert.h>
#include <libsystem/core/CString.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>
#include <libsystem/utils/NumberParser.h>

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
        stream_format(err_stream, "cd: Cannot access '%s': %s\n", new_directory, result_to_string(result));
        return -1;
    }

    return 0;
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

static ShellBuiltin _shell_builtins[] = {
    {"cd", shell_builtin_cd},
    {"exit", shell_builtin_exit},
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
