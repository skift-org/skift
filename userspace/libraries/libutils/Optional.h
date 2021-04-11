#pragma once

#include <assert.h>
#include <libutils/Move.h>
#include <libutils/New.h>

template <typename T>
class Optional
{
private:
    bool _present = false;

    union
    {
        T _storage;
    };

public:
    ALWAYS_INLINE bool present() const
    {
        return _present;
    }

    ALWAYS_INLINE T &unwrap()
    {
        assert(present());
        return _storage;
    }

    ALWAYS_INLINE const T &unwrap() const
    {
        assert(present());
        return _storage;
    }

    ALWAYS_INLINE T unwrap_or(const T &defaut_value) const
    {
        if (present())
        {
            return unwrap();
        }
        else
        {
            return defaut_value;
        }
    }

    ALWAYS_INLINE Optional() {}

    ALWAYS_INLINE Optional(const T &value)
    {
        _present = true;
        new (&_storage) T(value);
    }

    ALWAYS_INLINE Optional(T &&value)
    {
        _present = true;
        new (&_storage) T(move(value));
    }

    ALWAYS_INLINE Optional(const Optional &other)
    {
        if (other.present())
        {
            _present = true;
            new (&_storage) T(other.unwrap());
        }
    }

    ALWAYS_INLINE Optional(Optional &&other)
    {
        if (other.present())
        {
            new (&_storage) T(other.unwrap());
            _present = true;
        }
    }

    ALWAYS_INLINE Optional &operator=(const Optional &other)
    {
        if (this != &other)
        {
            clear();
            _present = other._present;
            if (other._present)
            {
                new (&_storage) T(other.unwrap());
            }
        }

        return *this;
    }

    ALWAYS_INLINE Optional &operator=(Optional &&other)
    {
        if (this != &other)
        {
            clear();
            _present = other._present;
            if (other._present)
            {
                new (&_storage) T(other.unwrap());
            }
        }

        return *this;
    }

    ALWAYS_INLINE bool operator==(const T &other) const
    {
        if (!present())
        {
            return false;
        }

        return unwrap() == other;
    }

    ALWAYS_INLINE ~Optional()
    {
        clear();
    }

    ALWAYS_INLINE void clear()
    {
        if (_present)
        {
            _storage.~T();
            _present = false;
        }
    }
};