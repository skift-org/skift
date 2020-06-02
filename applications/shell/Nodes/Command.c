#include "shell/Nodes.h"

void shell_node_command_destroy(ShellNodeCommand *node)
{
    free(node->command);
    list_destroy_with_callback(node->arguments, free);
}

ShellNode *shell_node_command_create(char *command, List *arguments)
{
    ShellNodeCommand *node = __create(ShellNodeCommand);

    node->command = command;
    node->arguments = arguments;
    node->destroy = (ShellNodeDestroyCallback)shell_node_command_destroy;

    return (ShellNode *)node;
}
