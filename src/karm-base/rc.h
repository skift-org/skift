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

    bool dying() {
        return _strong == 0;
    }

    _Rc *collect() {
        if (_strong == 0 && _weak == 0) {
            delete this;
            return nullptr;
        }

        return this;
    }

    _Rc *ref_strong() {
        _strong++;
        return this;
    }

    _Rc *deref_strong() {
        _strong--;
        return collect();
    }

    template <typename T>
    T &unwrap_strong() {
        return *static_cast<T *>(_unwrap());
    }

    _Rc *ref_weak() {
        _weak++;
        return this;
    }

    _Rc *deref_weak() {
        _weak--;
        return collect();
    }

    template <typename T>
    T *unwrap_weak() {
        if (_strong == 0) {
            Debug::panic("Dereferencing weak reference to dead object");
        }

        return *static_cast<T *>(_unwrap());
    }
};

template <typename T>
struct Rc : public _Rc {
    T _buf;
    Rc(T &&buf) : _buf(std::forward<T>(buf)) {}
    void *_unwrap() override { return &_buf; }
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

    constexpr Strong &operator=(Strong const &other) {
        return *this = Strong(other);
    }

    constexpr Strong &operator=(Strong &&other) {
        std::swap(_rc, other._rc);
        return *this;
    }

    constexpr T *operator->() const {
        if (!_rc) {
            Debug::panic("Deferencing moved from Strong<T>");
        }

        return &_rc->unwrap_strong<T>();
    }

    constexpr T &operator*() const {
        if (!_rc) {
            Debug::panic("Deferencing moved from Strong<T>");
        }

        return _rc->unwrap_strong<T>();
    }
};

template <typename T>
using OptStrong = Opt<Strong<T>>;

template <typename T, typename... Args>
constexpr static Strong<T> make_strong(Args... args) {
    return {new Rc<T>(T{std::forward<Args>(args)...})};
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

    constexpr Weak &operator=(Weak const &other) { return *this = Weak(other); }

    constexpr Weak &operator=(Weak &&other) {
        std::swap(_rc, other._rc);
        return *this;
    }

    operator bool() {
        return _rc && _rc->dying() && (_rc = _rc->deref_weak());
    }

    operator bool() const {
        return _rc && _rc->dying();
    }

    T *operator->() {
        if (!_rc)
            Debug::panic("Deferencing moved from Weak<T>");

        return _rc->unwrap_weak<T>();
    }

    T &unwrap() {
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
