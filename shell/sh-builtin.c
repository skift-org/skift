#include <skift/cstring.h>
#include <skift/process.h>
#include <skift/iostream.h>
#include <skift/error.h>

#include "shell.h"

int shell_builtin_cd(shell_t *shell, int argc, const char **argv)
{
    UNUSED(shell);

    if (argc == 2)
    {
        int result = process_set_cwd(argv[1]);
        
        if (result == -1)
        {
            iostream_printf(err_stream, "cd: cannot access '%s'", argv[1]);
            error_print("");
            return -1;
        }
        return result;
    }
    else
    {
        process_set_cwd("/");
        
        return 0;
    }

}

int shell_builtin_exit(shell_t *shell, int argc, const char **argv)
{
    UNUSED(shell);
    UNUSED(argc);
    UNUSED(argv);

    shell->do_continue = false;
    if (argc == 2)
    {
        return stoi(argv[1], 10);
    }
    else
    {
        return 0;
    }
}

shell_builtin_command_t shell_builtins[] = {
    { "cd",   shell_builtin_cd   },
    { "exit", shell_builtin_exit },
    { NULL,   NULL               }
};

shell_builtin_command_t *shell_get_builtins(void)
{
    return shell_builtins;
}

shell_builtin_handler_t shell_get_builtin(const char* name)
{
    for (int i = 0; shell_builtins[i].name != NULL; i++)
    {
        if (strcmp(shell_builtins[i].name, name) == 0)
        {
            return shell_builtins[i].handler;
        }
    }
    
    return NULL;
}