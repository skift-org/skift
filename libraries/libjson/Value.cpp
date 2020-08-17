#include <libjson/Json.h>
#include <libsystem/Assert.h>
#include <libsystem/core/CString.h>

namespace json
{

    Value *create_string(const char *string)
    {
        if (string == nullptr)
        {
            return create_nil();
        }

        Value *value = __create(Value);

        value->type = STRING;
        value->storage_string = strdup(string);

        return value;
    }

    Value *create_string_adopt(char *string)
    {
        if (string == nullptr)
        {
            return create_nil();
        }

        Value *value = __create(Value);

        value->type = STRING;
        value->storage_string = string;

        return value;
    }

    Value *create_integer(int integer)
    {
        Value *value = __create(Value);

        value->type = INTEGER;
        value->storage_integer = integer;

        return value;
    }

    Value *create_double(double double_)
    {
        Value *value = __create(Value);

        value->type = DOUBLE;
        value->storage_double = double_;

        return value;
    }

    Value *create_object()
    {
        Value *value = __create(Value);

        value->type = OBJECT;
        value->storage_object = hashmap_create_string_to_value();

        return value;
    }

    Value *create_array()
    {
        Value *value = __create(Value);

        value->type = ARRAY;
        value->storage_array = list_create();

        return value;
    }

    Value *create_boolean(bool boolean)
    {
        Value *value = __create(Value);

        if (boolean)
        {
            value->type = TRUE;
        }
        else
        {
            value->type = FALSE;
        }

        return value;
    }

    Value *create_nil()
    {
        Value *value = __create(Value);

        value->type = NIL;

        return value;
    }

    void destroy(Value *value)
    {
        if (!value)
            return;

        switch (value->type)
        {
        case STRING:
            free(value->storage_string);
            break;
        case OBJECT:
            hashmap_destroy_with_callback(value->storage_object, (HashMapDestroyValueCallback)destroy);
            break;
        case ARRAY:
            list_destroy_with_callback(value->storage_array, (ListDestroyElementCallback)destroy);
            break;
        default:
            break;
        }

        free(value);
    }

    /* --- Value members ---------------------------------------------------- */

    bool is(Value *value, Type type)
    {
        if (value == nullptr)
        {
            return type == NIL;
        }

        return value->type == type;
    }

    const char *string_value(Value *value)
    {
        assert(is(value, STRING));

        return value->storage_string;
    }

    int integer_value(Value *value)
    {
        if (is(value, INTEGER))
        {
            return value->storage_integer;
        }
        else if (is(value, INTEGER))
        {
            return value->storage_double;
        }
        else
        {
            ASSERT_NOT_REACHED();
        }
    }

    double double_value(Value *value)
    {
        if (is(value, INTEGER))
        {
            return value->storage_integer;
        }
        else if (is(value, INTEGER))
        {
            return value->storage_double;
        }
        else
        {
            ASSERT_NOT_REACHED();
        }
    }

    bool object_has(Value *object, const char *key)
    {
        assert(is(object, OBJECT));

        return hashmap_has(object->storage_object, key);
    }

    Value *object_get(Value *object, const char *key)
    {
        assert(is(object, OBJECT));

        return (Value *)hashmap_get(object->storage_object, key);
    }

    void object_put(Value *object, const char *key, Value *value)
    {
        assert(is(object, OBJECT));

        hashmap_remove_with_callback(object->storage_object, key, (HashMapDestroyValueCallback)destroy);

        hashmap_put(object->storage_object, key, value);
    }

    void object_remove(Value *object, const char *key)
    {
        assert(is(object, OBJECT));

        hashmap_remove_with_callback(object->storage_object, key, (HashMapDestroyValueCallback)destroy);
    }

    size_t array_length(Value *array)
    {
        assert(is(array, ARRAY));

        return array->storage_array->count();
    }

    Value *array_get(Value *array, size_t index)
    {
        assert(is(array, ARRAY));

        Value *value = nullptr;

        assert(list_peekat(array->storage_array, index, (void **)&value));

        return value;
    }

    void array_put(Value *array, size_t index, Value *value)
    {
        assert(is(array, ARRAY));

        list_insert(array->storage_array, index, value);
    }

    void array_append(Value *array, Value *value)
    {
        assert(is(array, ARRAY));

        list_pushback(array->storage_array, value);
    }

    void array_remove(Value *array, size_t index)
    {
        assert(is(array, ARRAY));

        list_remove_at_with_callback(array->storage_array, index, (HashMapDestroyValueCallback)destroy);
    }

} // namespace json
