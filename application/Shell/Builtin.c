#include <libsystem/assert.h>
#include <libsystem/convert.h>
#include <libsystem/cstring.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Process.h>

#include "Shell/Builtin.h"

int shell_builtin_cd(int argc, const char **argv)
{
    if (argc == 2)
    {
        int result = process_set_cwd(argv[1]);

        if (result < 0)
        {
            stream_printf(err_stream, "cd: Cannot access '%s'", argv[1]);
            error_print("");
            return -1;
        }

        return 0;
    }
    else
    {
        int result = process_set_cwd("/");

        if (result < 0)
        {
            stream_printf(err_stream, "cd: Cannot access '%s'", "/");
            error_print("");
            return -1;
        }

        return 0;
    }
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