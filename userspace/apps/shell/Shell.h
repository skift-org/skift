#pragma once

#include <libio/Handle.h>

#include "shell/Nodes.h"

Opt<String> find_command_path(String command);

typedef int (*ShellBuiltinCallback)(int argc, const char **argv);

struct ShellBuiltin
{
    const char *name;
    ShellBuiltinCallback handler;
};

ShellBuiltinCallback shell_get_builtin(const char *name);

ShellNode *shell_parse(const char *command_text);

int shell_eval(ShellNode *node, Ref<IO::Handle> instream, Ref<IO::Handle> outstream);
