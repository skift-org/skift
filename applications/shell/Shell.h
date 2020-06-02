#pragma once

#include "shell/Nodes.h"

typedef int (*ShellBuiltinCallback)(int argc, const char **argv);

typedef struct
{
    const char *name;
    ShellBuiltinCallback handler;
} ShellBuiltin;

ShellBuiltinCallback shell_get_builtin(const char *name);

ShellNode *shell_parse(char *command_text);

int shell_eval(ShellNode *node);
