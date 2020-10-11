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
    value->storage_object = new HashMap<String, Value *>();

    return value;
}

Value *create_array()
{
    Value *value = __create(Value);

    value->type = ARRAY;
    value->storage_array = new Vector<Value *>();

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
        value->storage_object->foreach ([](auto, auto value) {
            destroy(value);
            return Iteration::CONTINUE;
        });
        delete value->storage_object;
        break;
    case ARRAY:
        value->storage_array->foreach ([](auto value) {
            destroy(value);
            return Iteration::CONTINUE;
        });
        delete value->storage_array;
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

#if !defined(__KERNEL__)

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

#endif

bool object_has(Value *object, const String &key)
{
    assert(is(object, OBJECT));

    return object->storage_object->has_key(key);
}

Value *object_get(Value *object, const String &key)
{
    assert(is(object, OBJECT));

    return (*object->storage_object)[key];
}

void object_put(Value *object, const String &key, Value *value)
{
    assert(is(object, OBJECT));

    if (object->storage_object->has_key(key))
    {
        destroy((*object->storage_object)[key]);
    }

    (*object->storage_object)[key] = value;
}

void object_remove(Value *object, const String &key)
{
    assert(is(object, OBJECT));

    if (object->storage_object->has_key(key))
    {
        destroy((*object->storage_object)[key]);
        (*object->storage_object)[key] = nullptr;
    }
}

size_t array_length(Value *array)
{
    assert(is(array, ARRAY));

    return array->storage_array->count();
}

Value *array_get(Value *array, size_t index)
{
    assert(is(array, ARRAY));

    return (*array->storage_array)[index];
}

void array_put(Value *array, size_t index, Value *value)
{
    assert(is(array, ARRAY));
    array->storage_array->insert(index, value);
}

void array_append(Value *array, Value *value)
{
    assert(is(array, ARRAY));
    array->storage_array->push_back(value);
}

void array_remove(Value *array, size_t index)
{
    assert(is(array, ARRAY));

    destroy((*array->storage_array)[index]);
    array->storage_array->remove_index(index);
}

} // namespace json
