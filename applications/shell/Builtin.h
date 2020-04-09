#pragma once

typedef int (*ShellBuiltinCallback)(int argc, const char **argv);

typedef struct
{
    const char *name;
    ShellBuiltinCallback handler;
} ShellBuiltin;

ShellBuiltinCallback shell_get_builtin(const char *name);
