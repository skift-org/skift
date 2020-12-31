#include "shell/Nodes.h"

void shell_node_destroy(ShellNode *node)
{
    if (node->destroy)
    {
        node->destroy(node);
    }

    free(node);
}
