#pragma once

#include <karm-base/std.h>
#include <karm-meta/traits.h>

namespace Karm::Base
{

struct _Rc
{
    int _strong = 0;
    int _weak = 0;

    virtual ~_Rc() = default;
    virtual void *_unwrap() = 0;

    _Rc *collect()
    {
        if (_strong == 0 && _weak == 0)
        {
            delete this;
            return nullptr;
        }
        return this;
    }

    _Rc *ref_strong()
    {
        _strong++;
        return this;
    }

    _Rc *deref_strong()
    {
        _strong--;
        return collect();
    }

    template <typename T>
    T &unwrap_strong()
    {
        return *static_cast<T *>(_unwrap());
    }

    _Rc *ref_weak()
    {
        _weak++;
        return this;
    }

    _Rc *deref_weak()
    {
        _weak--;
        return collect();
    }

    template <typename T>
    T *unwrap_weak()
    {
        if (_strong == 0)
        {
            return nullptr;
        }

        return *static_cast<T *>(_unwrap());
    }
};

template <typename T>
struct Rc : _Rc
{
    T _buf;
    void *_unwrap() override { return &_buf; }
};

template <typename T>
struct Strong
{
    _Rc *_ptr = nullptr;

    constexpr Strong() = delete;

    constexpr Strong(_Rc *ptr) : _ptr(ptr->ref_strong()) {}

    template <Meta::Derive<T> U>
    constexpr Strong(Strong<U> const &other) : _ptr(_ref(other._ptr)) {}

    template <Meta::Derive<T> U>
    constexpr Strong(Strong<U> &&other) { std::swap(_ptr, other._ptr); }

    constexpr ~Strong() { _ptr = _ptr->deref_strong(); }

    constexpr Strong &operator=(Strong const &other) { return *this = Strong(other); }

    constexpr Strong &operator=(Strong &&other)
    {
        std::swap(_ptr, other._ptr);
        return *this;
    }

    constexpr T *operator->() { return &_ptr->unwrap_strong<T>(); }
    constexpr T &operator*() { return _ptr->unwrap_strong<T>(); }

    template <typename... Args>
    constexpr static Strong<T> make(Args... args)
    {
        return {new Rc<T>(T{std::forward<Args>(args)...})};
    }

    template <Meta::Derive<T> U, typename... Args>
    constexpr static Strong<T> make(Args... args)
    {
        return {new Rc<U>(U{std::forward<Args>(args)...})};
    }
};

/* TODO: Weak
template <typename T>
struct Weak
{
    _Rc *_ptr = nullptr;

    constexpr Weak() = delete;

    template <Meta::Derive<T> U>
    constexpr Weak(Weak<U> const &other) : _ptr(_ref(other._ptr)) {}

    template <Meta::Derive<T> U>
    constexpr Weak(Weak<U> &&other) { std::swap(_ptr, other._ptr); }

    constexpr ~Weak() { _ptr = _ptr->deref_weak(); }

    constexpr Weak &operator=(Weak const &other) { return *this = Weak(other); }

    constexpr Weak &operator=(Weak &&other)
    {
        std::swap(_ptr, other._ptr);
        return *this;
    }

    constexpr T *operator->() { return &_ptr->unwrap_weak<T>(); }
    constexpr T &operator*() { return _ptr->unwrap_weak<T>(); }
};
*/

} // namespace Karm::Base
