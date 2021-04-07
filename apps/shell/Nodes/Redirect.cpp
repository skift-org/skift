#include "shell/Nodes.h"

void shell_redirect_destroy(ShellRedirect *node)
{
    shell_node_destroy(node->command);
    free(node->destination);
}

ShellNode *shell_redirect_create(ShellNode *command, char *destination)
{
    ShellRedirect *node = CREATE(ShellRedirect);

    node->type = SHELL_NODE_REDIRECT;
    node->destroy = (ShellNodeDestroyCallback)shell_redirect_destroy;

    node->command = command;
    node->destination = destination;

    return (ShellNode *)node;
}
