#pragma once

#include <libsystem/utils/RefCounted.h>

template <typename T>
class RefPtr
{
private:
    T *_ptr;

public:
    enum AdoptTag
    {
        Adopt
    };

    RefPtr() : _ptr(nullptr) {}
    RefPtr(nullptr_t) : _ptr(nullptr) {}

    RefPtr(T &object) : _ptr(const_cast<T *>(&object)) { _ptr->ref(); }
    RefPtr(AdoptTag, T &object) : _ptr(const_cast<T *>(&object)) {}

    RefPtr(RefPtr &other) : _ptr(other.necked()) { _ptr->ref(); }
    RefPtr(AdoptTag, RefPtr &other) : _ptr(other.give_ref()) {}
    RefPtr(RefPtr &&other) : _ptr(other.give_ref()) {}

    template <typename U>
    RefPtr(RefPtr<U> &other) : _ptr(static_cast<T *>(other.necked())) { _ptr->ref(); }

    template <typename U>
    RefPtr(AdoptTag, RefPtr<U> &other) : _ptr(static_cast<T *>(other.give_ref())) {}

    template <typename U>
    RefPtr(RefPtr<U> &&other) : _ptr(static_cast<T *>(other.give_ref())) {}

    RefPtr &operator=(nullptr_t)
    {
        if (_ptr)
        {
            necked()->deref();
        }

        return *this;
    }

    RefPtr &operator=(RefPtr &other)
    {
        if (necked() != other.necked())
        {
            if (_ptr)
            {
                necked()->deref();
            }

            _ptr = other.necked();
            _ptr->ref();
        }

        return *this;
    }

    RefPtr &operator=(RefPtr &&other)
    {
        if (this != &other)
        {
            if (_ptr)
            {
                _ptr->deref();
            }

            _ptr = other.give_ref();
        }

        return *this;
    }

    template <typename U>
    RefPtr &operator=(RefPtr<U> &other)
    {
        if (necked() != other.necked())
        {
            if (necked())
            {
                necked()->deref();
            }

            _ptr = other.necked();
            necked()->ref();
        }

        return *this;
    }

    template <typename U>
    RefPtr &operator=(RefPtr<U> &&other)
    {
        if (this != static_cast<void *>(&other))
        {
            if (necked())
            {
                necked()->deref();
            }

            _ptr = other.give_ref();
        }

        return *this;
    }

    ~RefPtr()
    {
        if (_ptr)
            _ptr->deref();
    }

    T *operator->()
    {
        return _ptr;
    }

    T &operator*() { return *_ptr; }
    const T &operator*() const { return *_ptr; }

    bool operator==(RefPtr<T> other) const { return _ptr == other._ptr; }
    bool operator!=(RefPtr<T> other) const { return _ptr != other._ptr; }
    bool operator==(T *other) const { return _ptr == other; }
    bool operator!=(T *other) const { return _ptr != other; }

    operator bool() const
    {
        return _ptr != nullptr;
    }

    bool operator!() const
    {
        return _ptr == nullptr;
    }

    int refcount()
    {
        if (_ptr)
        {
            return _ptr->refcount();
        }
        else
        {
            return 0;
        }
    }

    T *give_ref()
    {
        T *ptr = _ptr;
        _ptr = nullptr;
        return ptr;
    }

    T *necked()
    {
        return _ptr;
    }
};

template <typename T>
inline RefPtr<T> adopt(T &object)
{
    return RefPtr<T>(RefPtr<T>::Adopt, object);
}

template <typename Type, typename... Args>
inline RefPtr<Type> make(Args &&... args)
{
    return RefPtr<Type>(adopt(*new Type(args...)));
}
