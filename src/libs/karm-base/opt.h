#pragma once

#include <karm-meta/callable.h>

#include "error.h"
#include "inert.h"
#include "macros.h"
#include "panic.h"
#include "std.h"
#include "try.h"

namespace Karm {

template <typename T>
struct Opt;

template <typename T>
struct [[nodiscard]] Opt {
    bool _present = false;
    Inert<T> _value{};

    ALWAYS_INLINE Opt() {}

    ALWAYS_INLINE Opt(None) {}

    template <typename U = T>
    ALWAYS_INLINE Opt(U &&value)
        requires(not Meta::Same<Meta::RemoveConstVolatileRef<U>, Opt<T>> and Meta::MoveConstructible<T, U>)
        : _present(true) {
        _value.ctor(std::forward<U>(value));
    }

    ALWAYS_INLINE Opt(Opt const &other)
        requires(Meta::CopyConstructible<T>)
        : _present(other._present) {
        if (_present) {
            _value.ctor(other.unwrap());
        }
    }

    ALWAYS_INLINE Opt(Opt &&other) : _present(other._present) {
        if (_present) {
            _value.ctor(other.take());
        }
    }

    ALWAYS_INLINE ~Opt() {
        clear();
    }

    ALWAYS_INLINE Opt &operator=(None) {
        clear();
        return *this;
    }

    ALWAYS_INLINE Opt &operator=(T const &value) {
        clear();
        _present = true;
        _value.ctor(value);
        return *this;
    }

    ALWAYS_INLINE Opt &operator=(T &&value) {
        clear();
        _present = true;
        _value.ctor(std::move(value));
        return *this;
    }

    ALWAYS_INLINE Opt &operator=(Opt const &other) {
        *this = Opt(other);
        return *this;
    }

    ALWAYS_INLINE Opt &operator=(Opt &&other) {
        clear();
        if (other._present) {
            _present = true;
            _value.ctor(other.take());
        }
        return *this;
    }

    ALWAYS_INLINE bool operator==(None) const {
        return not _present;
    }

    ALWAYS_INLINE operator bool() const {
        return _present;
    }

    ALWAYS_INLINE bool has() const {
        return _present;
    }

    ALWAYS_INLINE T *operator->() {
        if (not _present) {
            panic("Unwrapping None");
        }
        return &_value.unwrap();
    }

    ALWAYS_INLINE T &operator*() {
        if (not _present) {
            panic("Unwrapping None");
        }
        return _value.unwrap();
    }

    ALWAYS_INLINE T const *operator->() const {
        if (not _present) {
            panic("Unwrapping None");
        }
        return &_value.unwrap();
    }

    ALWAYS_INLINE T const &operator*() const {
        if (not _present) {
            panic("Unwrapping None");
        }
        return _value.unwrap();
    }

    template <typename... Args>
    ALWAYS_INLINE T &emplace(Args &&...args) {
        clear();
        _present = true;
        _value.ctor(std::forward<Args>(args)...);
        return _value.unwrap();
    }

    ALWAYS_INLINE void clear() {
        if (_present) {
            _value.dtor();
            _present = false;
        }
    }

    ALWAYS_INLINE None none() const {
        return NONE;
    }

    ALWAYS_INLINE T &unwrap(char const *msg = "unwraping none") {
        if (not _present) {
            panic(msg);
        }
        return _value.unwrap();
    }

    ALWAYS_INLINE T const &unwrap(char const *msg = "unwraping none") const {
        if (not _present) {
            panic(msg);
        }
        return _value.unwrap();
    }

    ALWAYS_INLINE T take() {
        if (not _present) {
            panic("taking from none");
        }
        T v = _value.take();
        _present = false;
        return v;
    }

    ALWAYS_INLINE auto visit(auto visitor)
        -> decltype(visitor(_value.unwrap())) {

        if (_present) {
            return visitor(_value.unwrap());
        }

        return {};
    }

    ALWAYS_INLINE auto visit(auto visitor) const
        -> decltype(visitor(_value.unwrap())) {

        if (_present) {
            return visitor(_value.unwrap());
        }

        return {};
    }

    // call operator
    template <typename... Args>
    ALWAYS_INLINE auto operator()(Args &&...args) {
        using OptRet = Opt<Meta::Ret<T, Args...>>;

        if constexpr (Meta::Same<void, Meta::Ret<T, Args...>>) {
            // Handle void return type
            if (not _present) {
                return false;
            }
            _value.unwrap()(std::forward<Args>(args)...);
            return true;
        } else {
            // Handle non-void return type
            if (not _present) {
                return OptRet{NONE};
            }

            return OptRet{_value.unwrap()(std::forward<Args>(args)...)};
        }
    }

    auto mapValue(auto f) -> Opt<decltype(f(unwrap()))> {
        if (_present) {
            return {f(unwrap())};
        }
        return {NONE};
    }
};

static_assert(Tryable<Opt<isize>>);

} // namespace Karm
