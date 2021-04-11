#include "shell/Nodes.h"

void shell_pipeline_destroy(ShellPipeline *node)
{
    list_destroy_with_callback(node->commands, (ListDestroyElementCallback)shell_node_destroy);
}

ShellNode *shell_pipeline_create(List *commands)
{
    ShellPipeline *node = CREATE(ShellPipeline);

    node->type = SHELL_NODE_PIPELINE;
    node->destroy = (ShellNodeDestroyCallback)shell_pipeline_destroy;
    node->commands = commands;

    return (ShellNode *)node;
}
