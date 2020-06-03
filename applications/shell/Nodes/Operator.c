#include "shell/Nodes.h"

void shell_node_operator_destroy(ShellNodeOperator *node)
{
    shell_node_destroy(node->left);
    shell_node_destroy(node->right);
}

ShellNode *shell_node_operator_create(ShellNodeType type, ShellNode *left, ShellNode *right)
{
    ShellNodeOperator *node = __create(ShellNodeOperator);

    node->type = type;
    node->left = left;
    node->right = right;
    node->destroy = (ShellNodeDestroyCallback)shell_node_operator_destroy;

    return (ShellNode *)node;
}
