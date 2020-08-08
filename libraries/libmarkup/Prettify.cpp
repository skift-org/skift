#include <libmarkup/Markup.h>
#include <libsystem/utils/BufferBuilder.h>

struct MarkupPrettifyState
{
    BufferBuilder *builder;
    int depth;
    bool color;
    bool first_line;
};

static const char *depth_color[] = {
    "\e[91m",
    "\e[92m",
    "\e[93m",
    "\e[94m",
    "\e[95m",
    "\e[96m",
};

static void markup_prettify_ident(MarkupPrettifyState *state)
{
    if (!state->first_line)
        buffer_builder_append_chr(state->builder, '\n');
    else
        state->first_line = false;

    for (int i = 0; i < state->depth; i++)
    {
        buffer_builder_append_str(state->builder, "    ");
    }
}

static void markup_prettify_internal(MarkupPrettifyState *state, MarkupNode *node)
{
    markup_prettify_ident(state);

    buffer_builder_append_chr(state->builder, '<');

    if (state->color)
    {
        buffer_builder_append_str(state->builder, depth_color[state->depth % 6]);
    }

    buffer_builder_append_str(state->builder, node->type);

    if (state->color)
    {
        buffer_builder_append_str(state->builder, "\e[m");
    }

    list_foreach(MarkupAttribute, attribute, node->attributes)
    {
        buffer_builder_append_chr(state->builder, ' ');
        buffer_builder_append_str(state->builder, attribute->name);

        if (attribute->value)
        {
            buffer_builder_append_chr(state->builder, '=');
            buffer_builder_append_chr(state->builder, '"');
            buffer_builder_append_str(state->builder, attribute->value);
            buffer_builder_append_chr(state->builder, '"');
        }
    }

    if (node->childs->empty())
    {
        buffer_builder_append_str(state->builder, "/>");
        return;
    }
    else
    {
        buffer_builder_append_chr(state->builder, '>');
    }

    list_foreach(MarkupNode, child, node->childs)
    {
        state->depth++;
        markup_prettify_internal(state, child);
        state->depth--;
    }

    markup_prettify_ident(state);
    buffer_builder_append_str(state->builder, "</");

    if (state->color)
    {
        buffer_builder_append_str(state->builder, depth_color[state->depth % 6]);
    }

    buffer_builder_append_str(state->builder, node->type);

    if (state->color)
    {
        buffer_builder_append_str(state->builder, "\e[m");
    }

    buffer_builder_append_chr(state->builder, '>');
}

char *markup_prettify(MarkupNode *root)
{
    BufferBuilder *builder = buffer_builder_create(128);

    MarkupPrettifyState state = {builder, 0, true, true};

    markup_prettify_internal(&state, root);

    return buffer_builder_finalize(builder);
}
