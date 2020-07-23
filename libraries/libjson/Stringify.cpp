#include <libjson/Json.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libsystem/utils/NumberFormatter.h>

void json_stringify_internal(BufferBuilder *builder, JsonValue *value);

IterationDecision json_stringify_object(BufferBuilder *builder, char *key, JsonValue *value)
{
    buffer_builder_append_str(builder, "\"");
    buffer_builder_append_str(builder, key);
    buffer_builder_append_str(builder, "\"");
    buffer_builder_append_str(builder, " : ");
    json_stringify_internal(builder, value);
    buffer_builder_append_str(builder, ", ");

    return ITERATION_CONTINUE;
}

IterationDecision json_stringify_array(BufferBuilder *builder, JsonValue *value)
{
    json_stringify_internal(builder, value);
    buffer_builder_append_str(builder, ", ");

    return ITERATION_CONTINUE;
}

void json_stringify_internal(BufferBuilder *builder, JsonValue *value)
{
    switch (value->type)
    {
    case JSON_STRING:
        buffer_builder_append_str(builder, "\"");
        buffer_builder_append_str(builder, value->storage_string);
        buffer_builder_append_str(builder, "\"");
        break;
    case JSON_INTEGER:
    {
        char buffer[128];
        format_int(FORMAT_DECIMAL, value->storage_integer, buffer, 128);
        buffer_builder_append_str(builder, buffer);

        break;
    }

    case JSON_DOUBLE:
    {
        char buffer[128];
        format_double(FORMAT_DECIMAL, value->storage_double, buffer, 128);
        buffer_builder_append_str(builder, buffer);

        break;
    }

    case JSON_OBJECT:
        buffer_builder_append_str(builder, "{");
        hashmap_iterate(value->storage_object, builder, (HashMapIterationCallback)json_stringify_object);
        buffer_builder_rewind(builder, 2); // remove the last ", "
        buffer_builder_append_str(builder, "}");
        break;
    case JSON_ARRAY:
        buffer_builder_append_str(builder, "[");
        list_iterate(value->storage_array, builder, (ListIterationCallback)json_stringify_array);
        buffer_builder_rewind(builder, 2); // remove the last ", "
        buffer_builder_append_str(builder, "]");
        break;
    case JSON_TRUE:
        buffer_builder_append_str(builder, "true");
        break;
    case JSON_FALSE:
        buffer_builder_append_str(builder, "false");
        break;
    case JSON_NULL:
        buffer_builder_append_str(builder, "null");
        break;

    default:
        break;
    }
}

char *json_stringify(JsonValue *value)
{
    BufferBuilder *builder = buffer_builder_create(128);

    json_stringify_internal(builder, value);

    return buffer_builder_finalize(builder);
}
