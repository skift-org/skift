#pragma once

#include <assert.h>

template <typename T>
class OwnPtr
{
private:
    T *_ptr = nullptr;

public:
    OwnPtr() {}
    OwnPtr(nullptr_t) {}

    OwnPtr(T *ptr) : _ptr(ptr) {}

    OwnPtr(const OwnPtr &other) : _ptr(const_cast<OwnPtr &>(other).give_ref()) {}

    OwnPtr(OwnPtr &&other) : _ptr(other.give_ref()) {}

    template <typename U>
    OwnPtr(const OwnPtr<U> &other) : _ptr(static_cast<U *>(const_cast<OwnPtr<U>>(other).give_ref())) {}

    template <typename U>
    OwnPtr(OwnPtr<U> &&other) : _ptr(static_cast<U *>(other.give_ref())) {}

    ~OwnPtr()
    {
        if (_ptr)
        {
            delete _ptr;
            _ptr = nullptr;
        }
    }

    OwnPtr &operator=(OwnPtr &other)
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
    OwnPtr &operator=(OwnPtr<U> &other)
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

    OwnPtr &operator=(OwnPtr &&other)
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
    OwnPtr &operator=(OwnPtr<U> &&other)
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

    bool operator==(const OwnPtr<T> &other) const
    {
        return _ptr == other._ptr;
    }

    template <typename U>
    bool operator==(const OwnPtr<U> &other) const
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
inline OwnPtr<Type> own(Args &&... args)
{
    return OwnPtr<Type>(new Type(forward<Args>(args)...));
}
