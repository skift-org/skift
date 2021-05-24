#include "shell/Nodes.h"

void shell_pipeline_destroy(ShellPipeline *node)
{
    for (auto *command : *node->commands)
    {
        shell_node_destroy(command);
    }

    delete node->commands;
}

ShellNode *shell_pipeline_create(List<ShellNode *> *commands)
{
    ShellPipeline *node = CREATE(ShellPipeline);

    node->type = SHELL_NODE_PIPELINE;
    node->destroy = (ShellNodeDestroyCallback)shell_pipeline_destroy;
    node->commands = commands;

    return (ShellNode *)node;
}
