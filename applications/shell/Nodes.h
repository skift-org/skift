#pragma once

#include <libsystem/utils/List.h>

typedef enum
{
    SHELL_OPERATOR_PIPE,
    SHELL_OPERATOR_AND,
    SHELL_OPERATOR_OR,
} ShellOperator;

typedef enum
{
    SHELL_NODE_COMMAND
} ShellNodeType;

struct ShellNode;

typedef void (*ShellNodeDestroyCallback)(struct ShellNode *node);

#define SHELL_NODE      \
    ShellNodeType type; \
    ShellNodeDestroyCallback destroy;

typedef struct ShellNode
{
    SHELL_NODE;
} ShellNode;

typedef struct
{
    SHELL_NODE;

    char *command;
    List /* of cstring */ *arguments;
} ShellNodeCommand;

typedef struct
{
    SHELL_NODE;

    ShellOperator operator_;
    ShellNode *left;
    ShellNode *right;
} ShellNodeInfixOperator;

ShellNode *shell_node_command_create(char *command, List *arguments);
