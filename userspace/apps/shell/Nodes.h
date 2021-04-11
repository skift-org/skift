#pragma once

#include <libsystem/utils/List.h>

enum ShellNodeType
{
    SHELL_NODE_NONE,
    SHELL_NODE_COMMAND,
    SHELL_NODE_PIPELINE,
    SHELL_NODE_REDIRECT,
};

struct ShellNode;

typedef void (*ShellNodeDestroyCallback)(struct ShellNode *node);

struct ShellNode
{
    ShellNodeType type;
    ShellNodeDestroyCallback destroy;
};

struct ShellCommand : public ShellNode
{
    char *command;
    List /* of cstring */ *arguments;
};

struct ShellPipeline : public ShellNode
{
    List *commands;
};

struct ShellRedirect : public ShellNode
{
    ShellNode *command;
    char *destination;
};

void shell_node_destroy(ShellNode *node);

ShellNode *shell_command_create(char *command, List *arguments);

ShellNode *shell_pipeline_create(List *commands);

ShellNode *shell_redirect_create(ShellNode *command, char *destination);
