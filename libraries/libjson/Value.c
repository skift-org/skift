#include <libjson/Json.h>
#include <libsystem/assert.h>
#include <libsystem/cstring.h>

JsonValue *json_create_string(const char *string)
{
    JsonValue *value = __create(JsonValue);

    value->type = JSON_STRING;
    value->storage_string = strdup(string);

    return value;
}

JsonValue *json_create_string_adopt(char *string)
{
    JsonValue *value = __create(JsonValue);

    value->type = JSON_STRING;
    value->storage_string = string;

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

JsonValue *json_create_boolean(bool value)
{
    if (value)
    {
        return json_create_true();
    }
    else
    {
        return json_create_false();
    }
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
    if (!value)
    {
        return;
    }

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

/* --- JsonValue members ---------------------------------------------------- */

bool json_is(JsonValue *value, JsonType type)
{
    if (value == NULL)
    {
        return false;
    }

    return value->type == type;
}

const char *json_string_value(JsonValue *value)
{
    assert(json_is(value, JSON_STRING));

    return value->storage_string;
}

int json_integer_value(JsonValue *value)
{
    if (json_is(value, JSON_INTEGER))
    {
        return value->storage_integer;
    }
    else if (json_is(value, JSON_INTEGER))
    {
        return value->storage_double;
    }
    else
    {
        ASSERT_NOT_REACHED();
    }
}

double json_double_value(JsonValue *value)
{
    if (json_is(value, JSON_INTEGER))
    {
        return value->storage_integer;
    }
    else if (json_is(value, JSON_INTEGER))
    {
        return value->storage_double;
    }
    else
    {
        ASSERT_NOT_REACHED();
    }
}

bool json_object_has(JsonValue *object, const char *key)
{
    assert(json_is(object, JSON_OBJECT));

    return hashmap_has(object->storage_object, key);
}

JsonValue *json_object_get(JsonValue *object, const char *key)
{
    assert(json_is(object, JSON_OBJECT));

    return (JsonValue *)hashmap_get(object->storage_object, key);
}

void json_object_put(JsonValue *object, const char *key, JsonValue *value)
{
    assert(json_is(object, JSON_OBJECT));

    hashmap_remove_with_callback(object->storage_object, key, (HashMapDestroyKeyCallback)json_destroy);

    hashmap_put(object->storage_object, key, value);
}

void json_object_remove(JsonValue *object, const char *key)
{
    assert(json_is(object, JSON_OBJECT));

    hashmap_remove_with_callback(object->storage_object, key, (HashMapDestroyKeyCallback)json_destroy);
}

size_t json_array_lenght(JsonValue *array)
{
    assert(json_is(array, JSON_ARRAY));

    return list_count(array->storage_array);
}

JsonValue *json_array_get(JsonValue *array, size_t index)
{
    assert(json_is(array, JSON_ARRAY));

    JsonValue *value = NULL;

    assert(list_peekat(array->storage_array, index, (void **)&value));

    return value;
}

void json_array_put(JsonValue *array, size_t index, JsonValue *value)
{
    assert(json_is(array, JSON_ARRAY));

    list_insert(array->storage_array, index, value);
}

void json_array_append(JsonValue *array, JsonValue *value)
{
    assert(json_is(array, JSON_ARRAY));

    list_pushback(array->storage_array, value);
}

void json_array_remove(JsonValue *array, size_t index)
{
    assert(json_is(array, JSON_ARRAY));

    list_remove_at_with_callback(array->storage_array, index, (HashMapDestroyKeyCallback)json_destroy);
}