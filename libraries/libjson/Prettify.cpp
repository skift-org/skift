#include <libjson/Json.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libsystem/utils/NumberFormatter.h>

namespace json
{
struct PrettifyState
{
    BufferBuilder *builder;
    int depth;
    bool color;
};

static const char *depth_color[] = {
    "\e[91m",
    "\e[92m",
    "\e[93m",
    "\e[94m",
    "\e[95m",
    "\e[96m",
};

static void prettify_internal(PrettifyState *state, Value *value);

static void prettify_ident(PrettifyState *state)
{
    buffer_builder_append_chr(state->builder, '\n');
    for (int i = 0; i < state->depth; i++)
    {
        buffer_builder_append_str(state->builder, "    ");
    }
}

static Iteration prettify_object(PrettifyState *state, String &key, Value *value)
{
    prettify_ident(state);

    if (state->color)
    {
        buffer_builder_append_str(state->builder, depth_color[state->depth % 6]);
    }

    buffer_builder_append_str(state->builder, "\"");
    buffer_builder_append_str(state->builder, key.cstring());
    buffer_builder_append_str(state->builder, "\"");

    if (state->color)
    {
        buffer_builder_append_str(state->builder, "\e[m");
    }

    buffer_builder_append_str(state->builder, ": ");
    prettify_internal(state, value);
    buffer_builder_append_str(state->builder, ",");

    return Iteration::CONTINUE;
}

static Iteration prettify_array(PrettifyState *state, Value *value)
{
    prettify_ident(state);
    prettify_internal(state, value);
    buffer_builder_append_str(state->builder, ",");

    return Iteration::CONTINUE;
}

static void prettify_internal(PrettifyState *state, Value *value)
{
    switch (value->type)
    {

    case STRING:
    {
        buffer_builder_append_str(state->builder, "\"");
        buffer_builder_append_str(state->builder, value->storage_string);
        buffer_builder_append_str(state->builder, "\"");
        break;
    }

    case INTEGER:
    {
        char buffer[128];
        format_int(FORMAT_DECIMAL, value->storage_integer, buffer, 128);
        buffer_builder_append_str(state->builder, buffer);
        break;
    }

#ifndef __KERNEL__

    case DOUBLE:
    {
        char buffer[128];
        format_double(FORMAT_DECIMAL, value->storage_double, buffer, 128);
        buffer_builder_append_str(state->builder, buffer);
        break;
    }

#endif

    case OBJECT:
    {
        buffer_builder_append_str(state->builder, "{");

        if (value->storage_object->count() > 0)
        {
            state->depth++;

            value->storage_object->foreach ([&](auto &key, auto &value) {
                prettify_object(state, key, value);
                return Iteration::CONTINUE;
            });

            buffer_builder_rewind(state->builder, 1); // remove the last ","
            state->depth--;
        }

        prettify_ident(state);
        buffer_builder_append_str(state->builder, "}");
        break;
    }

    case ARRAY:
    {
        buffer_builder_append_str(state->builder, "[");

        if (value->storage_array->count() > 0)
        {
            state->depth++;

            value->storage_array->foreach ([&](auto &value) {
                prettify_array(state, value);
                return Iteration::CONTINUE;
            });
            buffer_builder_rewind(state->builder, 1); // remove the last ","

            state->depth--;
        }

        prettify_ident(state);
        buffer_builder_append_str(state->builder, "]");
        break;
    }

    case TRUE:
    {
        buffer_builder_append_str(state->builder, "true");
        break;
    }

    case FALSE:
    {
        buffer_builder_append_str(state->builder, "false");
        break;
    }

    case NIL:
    {
        buffer_builder_append_str(state->builder, "null");
        break;
    }

    default:
        break;
    }
}

char *prettify(Value *value)
{
    BufferBuilder *builder = buffer_builder_create(128);

    PrettifyState state = {builder, 0, true};

    prettify_internal(&state, value);

    return buffer_builder_finalize(builder);
}
} // namespace json
