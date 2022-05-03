#pragma once

#include <karm-debug/panic.h>
#include <karm-meta/traits.h>

#include "_prelude.h"

#include "opt.h"
#include "ref.h"
#include "std.h"

namespace Karm::Base {

struct _Rc {
    int _strong = 0;
    int _weak = 0;

    virtual ~_Rc() = default;
    virtual void *_unwrap() = 0;

    auto dying() -> bool {
        return _strong == 0;
    }

    auto collect() -> _Rc * {
        if (_strong == 0 && _weak == 0) {
            delete this;
            return nullptr;
        }

        return this;
    }

    auto ref_strong() -> _Rc * {
        _strong++;
        return this;
    }

    auto deref_strong() -> _Rc * {
        _strong--;
        return collect();
    }

    template <typename T>
    auto unwrap_strong() -> T & {
        return *static_cast<T *>(_unwrap());
    }

    auto ref_weak() -> _Rc * {
        _weak++;
        return this;
    }

    auto deref_weak() -> _Rc * {
        _weak--;
        return collect();
    }

    template <typename T>
    auto unwrap_weak() -> _Rc * {
        if (_strong == 0) {
            Debug::panic("Dereferencing weak reference to dead object");
        }

        return *static_cast<T *>(_unwrap());
    }
};

template <typename T>
struct Rc : public _Rc {
    T _buf;
    template <typename... Args>
    Rc(Args &&...args) : _buf(std::forward<Args>(args)...) {}

    auto _unwrap() -> void * override { return &_buf; }
};

template <typename T>
struct Strong {
    _Rc *_rc = nullptr;

    constexpr Strong() = delete;

    constexpr Strong(_Rc *ptr) : _rc(ptr->ref_strong()) {}

    constexpr Strong(Strong const &other) : _rc(other._rc->ref_strong()) {}

    constexpr Strong(Strong &&other) : _rc(std::exchange(other._rc, nullptr)) {}

    template <Meta::Derive<T> U>
    constexpr Strong(Strong<U> const &other) : _rc(other._rc->ref_strong()) {}

    template <Meta::Derive<T> U>
    constexpr Strong(Strong<U> &&other) : _rc(std::exchange(other._rc, nullptr)) {}

    constexpr ~Strong() {
        if (_rc) {
            _rc = _rc->deref_strong();
        }
    }

    constexpr auto operator=(Strong const &other) -> Strong & {
        return *this = Strong(other);
    }

    constexpr auto operator=(Strong &&other) -> Strong & {
        std::swap(_rc, other._rc);
        return *this;
    }

    constexpr auto operator->() const -> T * {
        if (!_rc) {
            Debug::panic("Deferencing moved from Strong<T>");
        }

        return &_rc->unwrap_strong<T>();
    }

    constexpr auto operator*() const -> T & {
        if (!_rc) {
            Debug::panic("Deferencing moved from Strong<T>");
        }

        return _rc->unwrap_strong<T>();
    }
};

template <typename T>
using OptStrong = Opt<Strong<T>>;

template <typename T, typename... Args>
constexpr static auto make_strong(Args... args) -> Strong<T> {
    return {new Rc<T>(std::forward<Args>(args)...)};
}

template <typename T>
struct Weak {
    _Rc *_rc = nullptr;

    constexpr Weak() = delete;

    template <Meta::Derive<T> U>
    constexpr Weak(Strong<U> const &other) : _rc(other._rc->ref_weak()) {}

    template <Meta::Derive<T> U>
    constexpr Weak(Weak<U> const &other) : _rc(other._rc->ref_weak()) {}

    template <Meta::Derive<T> U>
    constexpr Weak(Weak<U> &&other) { std::swap(_rc, other._rc); }

    constexpr ~Weak() {
        if (_rc) {
            _rc->deref_weak();
        }
    }

    constexpr auto operator=(Weak const &other) -> Weak & {
        return *this = Weak(other);
    }

    constexpr auto operator=(Weak &&other) -> Weak & {
        std::swap(_rc, other._rc);
        return *this;
    }

    operator bool() {
        return _rc && _rc->dying() && (_rc = _rc->deref_weak());
    }

    operator bool() const {
        return _rc && _rc->dying();
    }

    auto operator->() -> T * {
        if (!_rc)
            Debug::panic("Deferencing moved from Weak<T>");

        return _rc->unwrap_weak<T>();
    }

    auto unwrap() -> T & {
        if (!_rc)
            Debug::panic("Deferencing moved from Weak<T>");

        return _rc->unwrap_weak<T>();
    }

    void visit(auto visitor) {
        if (*this) {
            visitor(**this);
        }
    }
};

template <typename T>
using OptWeak = Opt<Weak<T>>;

} // namespace Karm::Base
