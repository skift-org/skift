#include <libjson/Json.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libsystem/utils/NumberFormatter.h>

typedef struct
{
    BufferBuilder *builder;
    int depth;
    bool color;
} JsonPrettifyState;

static const char *depth_color[] = {
    "\e[91m",
    "\e[92m",
    "\e[93m",
    "\e[94m",
    "\e[95m",
    "\e[96m",
};

static void json_prettify_internal(JsonPrettifyState *state, JsonValue *value);

static void json_prettify_ident(JsonPrettifyState *state)
{
    buffer_builder_append_chr(state->builder, '\n');
    for (int i = 0; i < state->depth; i++)
    {
        buffer_builder_append_str(state->builder, "    ");
    }
}

static IterationDecision json_prettify_object(JsonPrettifyState *state, char *key, JsonValue *value)
{
    json_prettify_ident(state);

    if (state->color)
    {
        buffer_builder_append_str(state->builder, depth_color[state->depth % 6]);
    }

    buffer_builder_append_str(state->builder, "\"");
    buffer_builder_append_str(state->builder, key);
    buffer_builder_append_str(state->builder, "\"");

    if (state->color)
    {
        buffer_builder_append_str(state->builder, "\e[m");
    }

    buffer_builder_append_str(state->builder, ": ");
    json_prettify_internal(state, value);
    buffer_builder_append_str(state->builder, ",");

    return ITERATION_CONTINUE;
}

static IterationDecision json_prettify_array(JsonPrettifyState *state, JsonValue *value)
{
    json_prettify_ident(state);
    json_prettify_internal(state, value);
    buffer_builder_append_str(state->builder, ",");

    return ITERATION_CONTINUE;
}

static void json_prettify_internal(JsonPrettifyState *state, JsonValue *value)
{
    switch (value->type)
    {
    case JSON_STRING:
        buffer_builder_append_str(state->builder, "\"");
        buffer_builder_append_str(state->builder, value->storage_string);
        buffer_builder_append_str(state->builder, "\"");
        break;
    case JSON_INTEGER:
    {
        char buffer[128];
        format_int(FORMAT_DECIMAL, value->storage_integer, buffer, 128);
        buffer_builder_append_str(state->builder, buffer);

        break;
    }

    case JSON_DOUBLE:
    {
        char buffer[128];
        format_double(FORMAT_DECIMAL, value->storage_double, buffer, 128);
        buffer_builder_append_str(state->builder, buffer);

        break;
    }

    case JSON_OBJECT:
        buffer_builder_append_str(state->builder, "{");

        state->depth++;
        hashmap_iterate(value->storage_object, state, (HashMapIterationCallback)json_prettify_object);
        buffer_builder_rewind(state->builder, 1); // remove the last ","
        state->depth--;

        json_prettify_ident(state);
        buffer_builder_append_str(state->builder, "}");
        break;
    case JSON_ARRAY:
        buffer_builder_append_str(state->builder, "[");

        state->depth++;
        list_iterate(value->storage_array, state, (ListIterationCallback)json_prettify_array);
        buffer_builder_rewind(state->builder, 1); // remove the last ","
        state->depth--;

        json_prettify_ident(state);
        buffer_builder_append_str(state->builder, "]");
        break;
    case JSON_TRUE:
        buffer_builder_append_str(state->builder, "true");
        break;
    case JSON_FALSE:
        buffer_builder_append_str(state->builder, "false");
        break;
    case JSON_NULL:
        buffer_builder_append_str(state->builder, "null");
        break;

    default:
        break;
    }
}

char *json_prettify(JsonValue *value)
{
    BufferBuilder *builder = buffer_builder_create(128);

    JsonPrettifyState state = {builder, 0, true};

    json_prettify_internal(&state, value);

    return buffer_builder_finalize(builder);
}