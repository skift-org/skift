#include "shell/Nodes.h"

void shell_command_destroy(ShellCommand *node)
{
    free(node->command);
    for (auto *arg : *node->arguments)
    {
        free(arg);
    }

    delete node->arguments;
}

ShellNode *shell_command_create(char *command, List<char *> *arguments)
{
    ShellCommand *node = CREATE(ShellCommand);

    node->type = SHELL_NODE_COMMAND;
    node->command = command;
    node->arguments = arguments;
    node->destroy = (ShellNodeDestroyCallback)shell_command_destroy;

    return (ShellNode *)node;
}
