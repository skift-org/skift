#pragma once

#include <skift/runtime.h>

#define MAX_COMMAND_LENGHT 128

struct shell;

typedef int (*shell_builtin_handler_t)(struct shell *shell, int argc, const char **argv);

typedef struct
{
    const char *name;
    shell_builtin_handler_t handler;
} shell_builtin_command_t;

typedef struct shell
{
    bool do_continue;
    shell_builtin_command_t *builtins;

    char* command_string;
    int command_argc;
    char**command_argv;

    int command_exit_value;
} shell_t;

shell_builtin_command_t *shell_get_builtins(void);
shell_builtin_handler_t shell_get_builtin(const char* name);

void shell_prompt(shell_t* this);
int shell_readline(shell_t *this);
int shell_split(shell_t* this);
void shell_cleanup(shell_t* this);
bool shell_eval(shell_t* this);