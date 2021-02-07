#pragma once

#include <assert.h>
#include <libutils/Move.h>
#include <libutils/New.h>

template <typename T>
class Optional
{
private:
    bool _present = false;
    char _storage[sizeof(T)];

public:
    bool present() const
    {
        return _present;
    }

    T &value()
    {
        assert(present());
        return *reinterpret_cast<T *>(_storage);
    }

    const T &value() const
    {
        assert(present());
        return *reinterpret_cast<const T *>(_storage);
    }

    T take_value()
    {
        assert(present());

        T v = move(value());
        clear();

        return v;
    }

    T value_or(const T &defaut_value) const
    {
        if (present())
        {
            return value();
        }
        else
        {
            return defaut_value;
        }
    }

    Optional() {}

    Optional(const T &value)
    {
        _present = true;
        new (&_storage) T(value);
    }

    Optional(T &&value)
    {
        _present = true;
        new (&_storage) T(move(value));
    }

    Optional(const Optional &other)
    {
        if (other.present())
        {
            _present = true;
            new (&_storage) T(*other);
        }
    }

    Optional(Optional &&other)
    {
        if (other.present())
        {
            new (&_storage) T(other.take_value());
            _present = true;
        }
    }

    Optional &operator=(const Optional &other)
    {
        if (this != &other)
        {
            clear();
            _present = other._present;
            if (other._present)
            {
                new (_storage) T(*other);
            }
        }

        return *this;
    }

    Optional &operator=(Optional &&other)
    {
        if (this != &other)
        {
            clear();
            _present = other._present;
            if (other._present)
            {
                new (_storage) T(other.take_value());
            }
        }

        return *this;
    }

    bool operator==(const T &other) const
    {
        if (!present())
        {
            return false;
        }

        return value() == other;
    }

    ~Optional()
    {
        clear();
    }

    void clear()
    {
        if (_present)
        {
            value().~T();
            _present = false;
        }
    }

    operator bool() const { return _present; }

    T &operator*() { return value(); }

    const T &operator*() const { return value(); }

    T *operator->() { return &value(); }

    const T *operator->() const { return &value(); }
};