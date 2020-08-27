#pragma once

#include <libsystem/Common.h>
#include <libutils/RefCounted.h>

enum AdoptTag
{
    ADOPT
};

template <typename T>
class RefPtr
{
private:
    T *_ptr = nullptr;

public:
    RefPtr() {}
    RefPtr(nullptr_t) {}

    RefPtr(T &object) : _ptr(&object) { _ptr->ref(); }
    RefPtr(AdoptTag, T &object) : _ptr(const_cast<T *>(&object)) {}

    RefPtr(RefPtr &other) : _ptr(other.naked()) { _ptr->ref(); }
    RefPtr(AdoptTag, RefPtr &other) : _ptr(other.give_ref()) {}
    RefPtr(RefPtr &&other) : _ptr(other.give_ref()) {}

    template <typename U>
    RefPtr(RefPtr<U> &other) : _ptr(static_cast<T *>(other.naked())) { _ptr->ref(); }

    template <typename U>
    RefPtr(AdoptTag, RefPtr<U> &other) : _ptr(static_cast<T *>(other.give_ref())) {}

    template <typename U>
    RefPtr(RefPtr<U> &&other) : _ptr(static_cast<T *>(other.give_ref())) {}

    ~RefPtr()
    {
        if (_ptr)
            _ptr->deref();

        _ptr = nullptr;
    }

    RefPtr &operator=(nullptr_t)
    {
        if (_ptr)
        {
            _ptr->deref();
        }

        _ptr = nullptr;

        return *this;
    }

    RefPtr &operator=(RefPtr &other)
    {
        if (naked() != other.naked())
        {
            if (_ptr)
            {
                naked()->deref();
            }

            _ptr = other.naked();
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
        if (naked() != other.naked())
        {
            if (naked())
            {
                naked()->deref();
            }

            _ptr = other.naked();
            naked()->ref();
        }

        return *this;
    }

    template <typename U>
    RefPtr &operator=(RefPtr<U> &&other)
    {
        if (this != static_cast<void *>(&other))
        {
            if (naked())
            {
                naked()->deref();
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

    [[nodiscard]] T *give_ref()
    {
        T *ptr = _ptr;
        _ptr = nullptr;
        return ptr;
    }

    T *naked()
    {
        return _ptr;
    }
};

template <typename T>
inline RefPtr<T> adopt(T &object)
{
    return RefPtr<T>(AdoptTag::ADOPT, object);
}

template <typename Type, typename... Args>
inline RefPtr<Type> make(Args &&... args)
{
    return RefPtr<Type>(adopt(*new Type(forward<Args>(args)...)));
}
