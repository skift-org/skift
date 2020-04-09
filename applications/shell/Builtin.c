#include <libsystem/assert.h>
#include <libsystem/convert.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>

#include "shell/Builtin.h"

int shell_builtin_cd(int argc, const char **argv)
{
    const char *new_cwd = "/";

    if (argc == 2)
    {
        new_cwd = argv[1];
    }

    Result result = process_set_cwd(new_cwd);

    if (result != SUCCESS)
    {
        stream_printf(err_stream, "cd: Cannot access '%s': %s\n", new_cwd, result_to_string(result));
        return -1;
    }

    return 0;
}

int shell_builtin_exit(int argc, const char **argv)
{
    if (argc == 2)
    {
        process_exit(convert_string_to_uint(argv[1], strnlen(argv[1], 32), 10));
    }
    else
    {
        process_exit(0);
    }

    ASSERT_NOT_REACHED();
}

static ShellBuiltin _shell_builtins[] = {
    {"cd", shell_builtin_cd},
    {"exit", shell_builtin_exit},
    {NULL, NULL},
};

ShellBuiltinCallback shell_get_builtin(const char *name)
{
    for (int i = 0; _shell_builtins[i].name != NULL; i++)
    {
        if (strcmp(_shell_builtins[i].name, name) == 0)
        {
            return _shell_builtins[i].handler;
        }
    }

    return NULL;
}