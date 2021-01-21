#include <libsystem/json/Array.h>
#include <libsystem/json/Object.h>
#include <libsystem/json/Value.h>
#include <libutils/Move.h>

namespace json
{

String Value::as_string() const
{
    if (_type == STRING)
    {
        return String(RefPtr<StringStorage>{*_string});
    }
    else if (_type == TRUE)
    {
        return "true";
    }
    else if (_type == FALSE)
    {
        return "false";
    }
    else if (_type == NIL)
    {
        return "null";
    }
    else
    {
        ASSERT_NOT_REACHED();
    }
}

int Value::as_integer() const
{
    if (_type == INTEGER)
    {
        return _integer;
    }

#ifndef __KERNEL__

    else if (_type == DOUBLE)
    {
        return _double;
    }

#endif

    else if (_type == TRUE)
    {
        return 1;
    }
    else if (_type == FALSE)
    {
        return 0;
    }
    else if (_type == NIL)
    {
        return 0;
    }
    else
    {
        return 0;
    }
}

#ifndef __KERNEL__

double Value::as_double() const
{
    if (_type == INTEGER)
    {
        return _integer;
    }
    else if (_type == DOUBLE)
    {
        return _double;
    }
    else if (_type == TRUE)
    {
        return 1;
    }
    else if (_type == FALSE)
    {
        return 0;
    }
    else if (_type == NIL)
    {
        return 0;
    }
    else
    {
        return 0;
    }
}

#endif

const Object &Value::as_object() const
{
    assert(is(OBJECT));
    return *_object;
}

const Array &Value::as_array() const
{
    assert(is(ARRAY));
    return *_array;
}

bool Value::as_bool() const
{
    if (_type == INTEGER)
    {
        return _integer != 0;
    }

#ifndef __KERNEL__

    else if (_type == DOUBLE)
    {
        return _double != 0.0;
    }

#endif

    else if (_type == TRUE)
    {
        return true;
    }
    else if (_type == FALSE)
    {
        return false;
    }
    else if (_type == NIL)
    {
        return false;
    }
    else
    {
        return false;
    }
}

Value::Value()
{
    _type = NIL;
    _all = 0;
}

Value::Value(String &value)
{
    _type = STRING;
    _string = ref_if_not_null(value.underlying_storage().naked());
}

Value::Value(String &&value)
{
    _type = STRING;
    _string = ref_if_not_null(value.underlying_storage().naked());
}

Value::Value(const char *cstring)
{
    _type = STRING;
    _string = new StringStorage(cstring);
}

Value::Value(int value)
{
    _type = INTEGER;
    _integer = value;
}

#ifndef __KERNEL__

Value::Value(double value)
{
    _type = DOUBLE;
    _double = value;
}

#endif

Value::Value(const Object &object)
{
    _type = OBJECT;
    _object = new Object(object);
}

Value::Value(Object &&object)
{
    _type = OBJECT;
    _object = new Object(move(object));
}

Value::Value(const Array &array)
{
    _type = ARRAY;
    _array = new Array(array);
}

Value::Value(Array &&array)
{
    _type = ARRAY;
    _array = new Array(move(array));
}

Value::Value(bool boolean)
{
    if (boolean)
    {
        _type = TRUE;
    }
    else
    {
        _type = FALSE;
    }

    _all = 0;
}

Value::Value(nullptr_t)
{
    _type = NIL;
    _all = 0;
}

Value::Value(const Value &other)
{
    _type = other._type;

    switch (_type)
    {
    case STRING:
        _string = ref_if_not_null(other._string);
        break;

    case INTEGER:
        _integer = other._integer;
        break;

#ifndef __KERNEL__

    case DOUBLE:
        _double = other._double;
        break;

#endif

    case OBJECT:
        _object = new Object(*other._object);
        break;

    case ARRAY:
        _array = new Array(*other._array);
        break;

    default:
        break;
    }
}

Value::Value(Value &&other)
{
    _type = exchange_and_return_initial_value(other._type, NIL);
    _all = exchange_and_return_initial_value(other._all, 0);
}

Value::~Value()
{
    clear();
}

bool Value::has(String key) const
{
    if (is(OBJECT))
    {
        return _object->has_key(key);
    }
    else
    {
        return false;
    }
}

const Value &Value::get(String key) const
{
    assert(is(OBJECT));

    return _object->operator[](key);
}

void Value::put(String key, const Value &value) const
{
    assert(is(OBJECT));
    _object->operator[](key) = value;
}

void Value::remove(String key)
{
    assert(is(OBJECT));
    _object->remove_key(key);
}

size_t Value::length() const
{
    if (is(OBJECT))
    {
        return _object->count();
    }
    else if (is(ARRAY))
    {
        return _array->count();
    }
    else
    {
        return 0;
    }
}

const Value &Value::get(size_t index) const
{
    assert(is(ARRAY));

    return _array->operator[](index);
}

void Value::put(size_t index, const Value &value)
{
    assert(is(ARRAY));

    _array->insert(index, value);
}

void Value::remove(size_t index)
{
    assert(is(ARRAY));

    _array->remove_index(index);
}

void Value::append(const Value &value)
{
    assert(is(ARRAY));

    _array->push_back(value);
}

Value &Value::operator=(const Value &other)
{
    clear();

    _type = other._type;

    switch (_type)
    {
    case STRING:
        _string = ref_if_not_null(other._string);
        break;

    case INTEGER:
        _integer = other._integer;
        break;

#ifndef __KERNEL__

    case DOUBLE:
        _double = other._double;
        break;

#endif

    case OBJECT:
        _object = new Object(*other._object);
        break;

    case ARRAY:
        _array = new Array(*other._array);
        break;

    default:
        break;
    }

    return *this;
}

Value &Value::operator=(Value &&other)
{
    swap(_type, other._type);
    swap(_all, other._all);

    return *this;
}

void Value::clear()
{
    if (_type == STRING)
    {
        deref_if_not_null(_string);
    }
    else if (_type == OBJECT)
    {
        delete _object;
    }
    else if (_type == ARRAY)
    {
        delete _array;
    }

    _all = 0;
    _type = NIL;
}

} // namespace json
