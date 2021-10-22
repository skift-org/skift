#pragma once

#include <libutils/Shared.h>
#include <libutils/Std.h>
#include <libutils/Tags.h>
#include <libutils/Traits.h>

namespace Utils
{

template <typename T>
struct Ref
{
private:
    T *_ptr = nullptr;

public:
    Ref() {}

    Ref(nullptr_t) {}

    Ref(T &object) : _ptr(&object)
    {
        ref_if_not_null(_ptr);
    }

    Ref(AdoptTag, T &object) : _ptr(const_cast<T *>(&object)) {}

    Ref(const Ref &other) : _ptr(other.naked())
    {
        ref_if_not_null(_ptr);
    }

    Ref(AdoptTag, Ref &other) : _ptr(other.give_ref()) {}

    Ref(Ref &&other) : _ptr(other.give_ref()) {}

    template <typename U>
    Ref(const Ref<U> &other) : _ptr(static_cast<T *>(other.naked()))
    {
        ref_if_not_null(_ptr);
    }

    template <typename U>
    Ref(AdoptTag, Ref<U> &other) : _ptr(static_cast<T *>(other.give_ref())) {}

    template <typename U>
    Ref(Ref<U> &&other) : _ptr(static_cast<T *>(other.give_ref())) {}

    ~Ref()
    {
        deref_if_not_null(_ptr);
        _ptr = nullptr;
    }

    Ref &operator=(nullptr_t)
    {
        deref_if_not_null(_ptr);
        _ptr = nullptr;

        return *this;
    }

    Ref &operator=(const Ref &other)
    {
        if (_ptr != other.naked())
        {
            deref_if_not_null(_ptr);
            _ptr = other.naked();
            ref_if_not_null(_ptr);
        }

        return *this;
    }

    Ref &operator=(Ref &&other)
    {
        if (this != &other)
        {
            deref_if_not_null(_ptr);
            _ptr = other.give_ref();
        }

        return *this;
    }

    template <typename U>
    Ref &operator=(Ref<U> &other)
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
    Ref &operator=(Ref<U> &&other)
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

    bool operator==(const Ref<T> other) const { return _ptr == other._ptr; }

    bool operator!=(const Ref<T> other) const { return _ptr != other._ptr; }

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
struct CallableRef : public Ref<T>
{
public:
    template <typename... TArgs>
    auto operator()(TArgs &&...args)
    {
        return (*this->naked())(std::forward<TArgs>(args)...);
    }
};

template <typename T>
inline Ref<T> adopt(T &object)
{
    return Ref<T>(ADOPT, object);
}

template <typename Type, typename... Args>
inline Ref<Type> make(Args &&...args)
{
    return Ref<Type>(adopt(*new Type(std::forward<Args>(args)...)));
}

template <typename Type, typename... Args>
inline CallableRef<Type> make_callable(Args &&...args)
{
    return CallableRef<Type>(adopt(*new Type(std::forward<Args>(args)...)));
}

template <typename T>
struct TrimRef;

template <typename T>
struct TrimRef<Ref<T>>
{
    typedef T type;
};

template <typename T>
struct IsRef : public FalseType
{
};

template <typename T>
struct IsRef<Ref<T>> : public TrueType
{
};

} // namespace Utils
