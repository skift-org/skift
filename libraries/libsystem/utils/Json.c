#include <libsystem/cstring.h>
#include <libsystem/utils/BufferBuilder.h>
#include <libsystem/utils/Json.h>

JsonValue *json_create_string(char *string)
{
    JsonValue *value = __create(JsonValue);

    value->type = JSON_STRING;
    value->storage_string = strdup(string);

    return value;
}

JsonValue *json_create_integer(int integer)
{
    JsonValue *value = __create(JsonValue);

    value->type = JSON_INTEGER;
    value->storage_integer = integer;

    return value;
}

JsonValue *json_create_double(double double_)
{
    JsonValue *value = __create(JsonValue);

    value->type = JSON_DOUBLE;
    value->storage_double = double_;

    return value;
}

JsonValue *json_create_object(void)
{
    JsonValue *value = __create(JsonValue);

    value->type = JSON_OBJECT;
    value->storage_object = hashmap_create_string_to_value();

    return value;
}

JsonValue *json_create_array(void)
{
    JsonValue *value = __create(JsonValue);

    value->type = JSON_ARRAY;
    value->storage_array = list_create();

    return value;
}

JsonValue *json_create_true(void)
{
    JsonValue *value = __create(JsonValue);

    value->type = JSON_TRUE;

    return value;
}

JsonValue *json_create_false(void)
{
    JsonValue *value = __create(JsonValue);

    value->type = JSON_FALSE;

    return value;
}

JsonValue *json_create_null(void)
{
    JsonValue *value = __create(JsonValue);

    value->type = JSON_NULL;

    return value;
}

void json_destroy(JsonValue *value)
{
    switch (value->type)
    {
    case JSON_STRING:
        free(value->storage_string);
        break;
    case JSON_OBJECT:
        hashmap_destroy_with_callback(value->storage_object, (HashMapDestroyValueCallback)json_destroy);
        break;
    case JSON_ARRAY:
        list_destroy_with_callback(value->storage_array, (ListDestroyElementCallback)json_destroy);
        break;
    default:
        break;
    }

    free(value);
}


void json_stringify_internal(BufferBuilder *builder, JsonValue *value);

IterationDecision json_stringify_object(BufferBuilder* builder, char* key, JsonValue* value) 
{
    buffer_builder_append_str(builder, key);
    buffer_builder_append_str(builder, " : ");
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
        break;
    case JSON_DOUBLE:
        break;
    case JSON_OBJECT:
        buffer_builder_append_str(builder, "{");
        buffer_builder_append_str(builder, "}");
        break;
    case JSON_ARRAY:
        buffer_builder_append_str(builder, "[");
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
    BufferBuilder* builder = buffer_builder_create(128);

    json_stringify_internal(builder, value);

    return buffer_builder_finalize(builder);
}
