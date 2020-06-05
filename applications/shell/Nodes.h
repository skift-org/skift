#pragma once

#include <libsystem/utils/List.h>

typedef enum
{
    SHELL_NODE_NONE,
    SHELL_NODE_COMMAND,
    SHELL_NODE_PIPELINE,
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
} ShellCommand;

typedef struct
{
    SHELL_NODE;

    List *commands;
} ShellPipeline;

void shell_node_destroy(ShellNode *node);

ShellNode *shell_command_create(char *command, List *arguments);

ShellNode *shell_pipeline_create(List *commands);
