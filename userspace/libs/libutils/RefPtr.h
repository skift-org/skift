#pragma once

#include <libutils/RefCounted.h>
#include <libutils/Std.h>
#include <libutils/Tags.h>
#include <libutils/Traits.h>

namespace Utils
{

template <typename T>
struct RefPtr
{
private:
    T *_ptr = nullptr;

public:
    RefPtr() {}

    RefPtr(nullptr_t) {}

    RefPtr(T &object) : _ptr(&object)
    {
        ref_if_not_null(_ptr);
    }

    RefPtr(AdoptTag, T &object) : _ptr(const_cast<T *>(&object)) {}

    RefPtr(const RefPtr &other) : _ptr(other.naked())
    {
        ref_if_not_null(_ptr);
    }

    RefPtr(AdoptTag, RefPtr &other) : _ptr(other.give_ref()) {}

    RefPtr(RefPtr &&other) : _ptr(other.give_ref()) {}

    template <typename U>
    RefPtr(const RefPtr<U> &other) : _ptr(static_cast<T *>(other.naked()))
    {
        ref_if_not_null(_ptr);
    }

    template <typename U>
    RefPtr(AdoptTag, RefPtr<U> &other) : _ptr(static_cast<T *>(other.give_ref())) {}

    template <typename U>
    RefPtr(RefPtr<U> &&other) : _ptr(static_cast<T *>(other.give_ref())) {}

    ~RefPtr()
    {
        deref_if_not_null(_ptr);
        _ptr = nullptr;
    }

    RefPtr &operator=(nullptr_t)
    {
        deref_if_not_null(_ptr);
        _ptr = nullptr;

        return *this;
    }

    RefPtr &operator=(const RefPtr &other)
    {
        if (_ptr != other.naked())
        {
            deref_if_not_null(_ptr);
            _ptr = other.naked();
            ref_if_not_null(_ptr);
        }

        return *this;
    }

    RefPtr &operator=(RefPtr &&other)
    {
        if (this != &other)
        {
            deref_if_not_null(_ptr);
            _ptr = other.give_ref();
        }

        return *this;
    }

    template <typename U>
    RefPtr &operator=(RefPtr<U> &other)
    {
        if (_ptr != other.naked())
        {
            deref_if_not_null(_ptr);
            _ptr = other.naked();
            ref_if_not_null(_ptr);
        }

        return *this;
    }

    template <typename U>
    RefPtr &operator=(RefPtr<U> &&other)
    {
        if (this != static_cast<void *>(&other))
        {
            deref_if_not_null(_ptr);
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

    bool operator==(const RefPtr<T> other) const { return _ptr == other._ptr; }

    bool operator!=(const RefPtr<T> other) const { return _ptr != other._ptr; }

    bool operator==(const T *other) const { return _ptr == other; }

    bool operator!=(const T *other) const { return _ptr != other; }

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

    T *naked() const
    {
        return _ptr;
    }
};

template <typename T>
struct CallableRefPtr : public RefPtr<T>
{
public:
    template <typename... TArgs>
    auto operator()(TArgs &&...args)
    {
        return (*this->naked())(std::forward<TArgs>(args)...);
    }
};

template <typename T>
inline RefPtr<T> adopt(T &object)
{
    return RefPtr<T>(ADOPT, object);
}

template <typename Type, typename... Args>
inline RefPtr<Type> make(Args &&...args)
{
    return RefPtr<Type>(adopt(*new Type(std::forward<Args>(args)...)));
}

template <typename Type, typename... Args>
inline CallableRefPtr<Type> make_callable(Args &&...args)
{
    return CallableRefPtr<Type>(adopt(*new Type(std::forward<Args>(args)...)));
}

template <typename T>
struct TrimRefPtr;

template <typename T>
struct TrimRefPtr<RefPtr<T>>
{
    typedef T type;
};

template <typename T>
struct IsRefPtr : public FalseType
{
};

template <typename T>
struct IsRefPtr<RefPtr<T>> : public TrueType
{
};

} // namespace Utils
