#include "shell/Nodes.h"

void shell_command_destroy(ShellCommand *node)
{
    free(node->command);
    list_destroy_with_callback(node->arguments, free);
}

ShellNode *shell_command_create(char *command, List *arguments)
{
    ShellCommand *node = CREATE(ShellCommand);

    node->type = SHELL_NODE_COMMAND;
    node->command = command;
    node->arguments = arguments;
    node->destroy = (ShellNodeDestroyCallback)shell_command_destroy;

    return (ShellNode *)node;
}
