#pragma once

#include <libutils/HashMap.h>
#include <libutils/Move.h>
#include <libutils/String.h>
#include <libutils/Vector.h>

namespace json
{

enum Type
{
    ERROR = -1,

    STRING,
    INTEGER,

#ifndef __KERNEL__
    DOUBLE,
#endif

    OBJECT,
    ARRAY,
    TRUE,
    FALSE,
    NIL,

    __TYPE_COUNT,
};

class Value
{
public:
    struct Array : public Vector<Value>
    {
    };

    struct Object : public HashMap<String, Value>
    {
    };

private:
    Type _type;

    union
    {
        StringStorage *_string;
        int _integer;
#ifndef __KERNEL__
        double _double;
#endif
        Object *_object;
        Array *_array;

        uint64_t _all;

        static_assert(sizeof(uint64_t) >= sizeof(void *));
        static_assert(sizeof(uint64_t) >= sizeof(double));
    };

public:
    inline bool is(Type type) const { return _type == type; }

    inline String as_string() const
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

    inline int as_integer() const
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
    inline double as_double() const
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

    inline const Object &as_object() const
    {
        assert(is(OBJECT));
        return *_object;
    }

    inline const Array &as_array() const
    {
        assert(is(ARRAY));
        return *_array;
    }

    inline bool as_bool() const
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

    inline Value()
    {
        _type = NIL;
        _all = 0;
    }

    inline Value(String &value)
    {
        _type = STRING;
        _string = ref_if_not_null(value.underlying_storage().naked());
    }

    inline Value(String &&value)
    {
        _type = STRING;
        _string = ref_if_not_null(value.underlying_storage().naked());
    }

    inline Value(const char *cstring)
    {
        _type = STRING;
        _string = new StringStorage(cstring);
    }

    inline Value(int value)
    {
        _type = INTEGER;
        _integer = value;
    }

#ifndef __KERNEL__

    inline Value(double value)
    {
        _type = DOUBLE;
        _double = value;
    }

#endif

    inline Value(const Object &object)
    {
        _type = OBJECT;
        _object = new Object(object);
    }

    inline Value(Object &&object)
    {
        _type = OBJECT;
        _object = new Object(move(object));
    }

    inline Value(const Array &array)
    {
        _type = ARRAY;
        _array = new Array(array);
    }

    inline Value(Array &&array)
    {
        _type = ARRAY;
        _array = new Array(move(array));
    }

    inline Value(bool boolean)
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

    inline Value(nullptr_t)
    {
        _type = NIL;
        _all = 0;
    }

    inline Value(const Value &other)
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

    inline Value(Value &&other)
    {
        _type = exchange_and_return_initial_value(other._type, NIL);
        _all = exchange_and_return_initial_value(other._all, 0);
    }

    inline ~Value()
    {
        clear();
    }

    template <typename TCallback>
    inline void with(String key, TCallback callback) const
    {
        if (has(key))
        {
            callback(get(key));
        }
    }

    inline bool has(String key) const
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

    inline const Value &get(String key) const
    {
        assert(is(OBJECT));

        return _object->operator[](key);
    }

    inline void put(String key, const Value &value) const
    {
        assert(is(OBJECT));
        _object->operator[](key) = value;
    }

    inline void remove(String key)
    {
        assert(is(OBJECT));
        _object->remove_key(key);
    }

    inline size_t length() const
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

    inline const Value &get(size_t index) const
    {
        assert(is(ARRAY));

        return _array->operator[](index);
    }

    inline void put(size_t index, const Value &value)
    {
        assert(is(ARRAY));

        _array->insert(index, value);
    }

    inline void remove(size_t index)
    {
        assert(is(ARRAY));

        _array->remove_index(index);
    }

    inline void append(const Value &value)
    {
        assert(is(ARRAY));

        _array->push_back(value);
    }

    inline Value &operator=(const Value &other)
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

    inline Value &operator=(Value &&other)
    {
        swap(_type, other._type);
        swap(_all, other._all);

        return *this;
    }

    inline void clear()
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
};

} // namespace json
