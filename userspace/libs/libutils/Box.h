#pragma once

#include <assert.h>
#include <libutils/Std.h>

namespace Utils
{

template <typename T>
struct Box
{
private:
    T *_ptr = nullptr;

public:
    Box() {}
    Box(nullptr_t) {}

    Box(T *ptr) : _ptr(ptr) {}

    Box(const Box &other) : _ptr(const_cast<Box &>(other).give_ref()) {}

    Box(Box &&other) : _ptr(other.give_ref()) {}

    template <typename U>
    Box(const Box<U> &other) : _ptr(static_cast<U *>(const_cast<Box<U>>(other).give_ref())) {}

    template <typename U>
    Box(Box<U> &&other) : _ptr(static_cast<U *>(other.give_ref())) {}

    ~Box()
    {
        if (_ptr)
        {
            delete _ptr;
            _ptr = nullptr;
        }
    }

    Box &operator=(Box &other)
    {
        if (naked() != other.naked())
        {
            if (_ptr)
            {
                delete _ptr;
            }

            _ptr = other.give_ref();
        }

        return *this;
    }

    template <typename U>
    Box &operator=(Box<U> &other)
    {
        if (naked() != other.naked())
        {
            if (_ptr)
            {
                delete _ptr;
            }

            _ptr = other.give_ref();
        }

        return *this;
    }

    Box &operator=(Box &&other)
    {
        if (this != &other)
        {
            if (_ptr)
            {
                delete _ptr;
            }

            _ptr = other.give_ref();
        }

        return *this;
    }

    template <typename U>
    Box &operator=(Box<U> &&other)
    {
        if (this != static_cast<void *>(&other))
        {
            if (_ptr)
            {
                delete _ptr;
            }

            _ptr = other.give_ref();
        }

        return *this;
    }

    T *operator->() const
    {
        assert(_ptr);
        return _ptr;
    }

    T &operator*() { return *_ptr; }

    const T &operator*() const { return *_ptr; }

    bool operator==(const Box<T> &other) const
    {
        return _ptr == other._ptr;
    }

    template <typename U>
    bool operator==(const Box<U> &other) const
    {
        return _ptr == static_cast<U *>(other._ptr);
    }

    bool operator==(T *other) const
    {
        return _ptr == other;
    }

    operator bool() const
    {
        return _ptr != nullptr;
    }

    bool operator!() const
    {
        return _ptr == nullptr;
    }

    [[nodiscard]] T *give_ref()
    {
        auto ref = _ptr;
        _ptr = nullptr;

        return ref;
    }

    T *naked()
    {
        return _ptr;
    }
};

template <typename Type, typename... Args>
inline Box<Type> own(Args &&...args)
{
    return Box<Type>(new Type(std::forward<Args>(args)...));
}

} // namespace Utils
