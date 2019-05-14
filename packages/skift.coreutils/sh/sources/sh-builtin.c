#include <skift/__plugs__.h>
#include <skift/cstring.h>

#include "shell.h"

int shell_builtin_cd(shell_t *shell, int argc, const char **argv)
{
    UNUSED(shell);

    if (argc == 2)
    {
        if (__plug_process_set_workdir(argv[1]))
        {
            return 0;
        }
        else
        {
            printf("File '%s' not found!\n", argv[1]);
            return 1;
        }
    }
    else
    {
        __plug_process_set_workdir("/");
        
        return 0;
    }

}

int shell_builtin_exit(shell_t *shell, int argc, const char **argv)
{
    UNUSED(shell);
    UNUSED(argc);
    UNUSED(argv);
    // FIXME: exit status
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