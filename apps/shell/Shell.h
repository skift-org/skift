#pragma once

#include "shell/Nodes.h"

bool find_command_path(char *buffer, const char *command);

typedef int (*ShellBuiltinCallback)(int argc, const char **argv);

struct ShellBuiltin
{
    const char *name;
    ShellBuiltinCallback handler;
};

ShellBuiltinCallback shell_get_builtin(const char *name);

ShellNode *shell_parse(char *command_text);

int shell_eval(ShellNode *node, Stream *in, Stream *out);
