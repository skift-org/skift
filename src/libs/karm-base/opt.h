#pragma once

#include <karm-meta/callable.h>
#include <karm-meta/traits.h>

#include "error.h"
#include "inert.h"
#include "macros.h"
#include "std.h"
#include "try.h"

namespace Karm {

template <typename T>
struct Opt;

template <typename T>
struct [[nodiscard]] Opt {
    struct _Empty {};

    bool _present;
    union {
        _Empty _empty;
        T _value;
    };

    ALWAYS_INLINE constexpr Opt()
        : _present(false), _empty() {}

    ALWAYS_INLINE constexpr Opt(None)
        : _present(false), _empty() {}

    template <typename U = T>
    ALWAYS_INLINE constexpr Opt(U &&value)
        requires(not Meta::Same<Meta::RemoveConstVolatileRef<U>, Opt<T>> and Meta::MoveConstructible<T, U>)
        : _present(true), _value(std::forward<U>(value)) {}

    ALWAYS_INLINE constexpr Opt(Opt const &other)
        requires(Meta::CopyConstructible<T>)
        : _present(other._present) {
        if (_present)
            std::construct_at(&_value, other.unwrap());
    }

    ALWAYS_INLINE constexpr Opt(Opt &&other) : _present(other._present) {
        if (_present)
            std::construct_at(&_value, other.take());
    }

    ALWAYS_INLINE constexpr ~Opt() {
        clear();
    }

    ALWAYS_INLINE constexpr Opt &operator=(None) {
        clear();
        return *this;
    }

    ALWAYS_INLINE constexpr Opt &operator=(T const &value) {
        clear();
        _present = true;
        std::construct_at(&_value, value);
        return *this;
    }

    ALWAYS_INLINE constexpr Opt &operator=(T &&value) {
        clear();
        _present = true;
        std::construct_at(&_value, std::move(value));
        return *this;
    }

    ALWAYS_INLINE constexpr Opt &operator=(Opt const &other) {
        *this = Opt(other);
        return *this;
    }

    ALWAYS_INLINE constexpr Opt &operator=(Opt &&other) {
        clear();
        if (other._present) {
            _present = true;
            std::construct_at(&_value, other.take());
        }
        return *this;
    }

    ALWAYS_INLINE constexpr bool operator==(None) const {
        return not _present;
    }

    ALWAYS_INLINE constexpr explicit operator bool() const {
        return _present;
    }

    ALWAYS_INLINE constexpr bool has() const {
        return _present;
    }

    ALWAYS_INLINE constexpr T *operator->() {
        if (not _present) {
            panic("Unwrapping None");
        }
        return &_value;
    }

    ALWAYS_INLINE constexpr T &operator*() {
        if (not _present) {
            panic("Unwrapping None");
        }
        return _value;
    }

    ALWAYS_INLINE constexpr T const *operator->() const {
        if (not _present) {
            panic("Unwrapping None");
        }
        return &_value;
    }

    ALWAYS_INLINE constexpr T const &operator*() const {
        if (not _present) {
            panic("Unwrapping None");
        }
        return _value;
    }

    template <typename... Args>
    ALWAYS_INLINE constexpr T &emplace(Args &&...args) {
        clear();
        _present = true;
        std::construct_at(&_value, std::forward<Args>(args)...);
        return _value;
    }

    ALWAYS_INLINE constexpr void clear() {
        if (_present) {
            _value.~T();
            _present = false;
        }
    }

    ALWAYS_INLINE constexpr None none() const {
        return NONE;
    }

    ALWAYS_INLINE constexpr T &unwrap(char const *msg = "unwraping none") {
        if (not _present) {
            panic(msg);
        }
        return _value;
    }

    ALWAYS_INLINE constexpr T const &unwrap(char const *msg = "unwraping none") const {
        if (not _present)
            panic(msg);
        return _value;
    }

    ALWAYS_INLINE constexpr T take(char const *msg = "unwraping none") {
        if (not _present)
            panic(msg);
        T v = std::move(_value);
        clear();
        return v;
    }

    ALWAYS_INLINE constexpr auto visit(auto visitor)
        -> decltype(visitor(_value)) {
        if (_present)
            return visitor(_value);
        return {};
    }

    ALWAYS_INLINE constexpr auto visit(auto visitor) const
        -> decltype(visitor(_value)) {
        if (_present)
            return visitor(_value);
        return {};
    }

    // call operator
    template <typename... Args>
    ALWAYS_INLINE constexpr auto operator()(Args &&...args) {
        using OptRet = Opt<Meta::Ret<T, Args...>>;

        if constexpr (Meta::Same<void, Meta::Ret<T, Args...>>) {
            // Handle void return type
            if (not _present) {
                return false;
            }
            _value(std::forward<Args>(args)...);
            return true;
        } else {
            // Handle non-void return type
            if (not _present) {
                return OptRet{NONE};
            }

            return OptRet{_value(std::forward<Args>(args)...)};
        }
    }

    ALWAYS_INLINE constexpr auto mapValue(auto f) -> Opt<decltype(f(unwrap()))> {
        if (_present) {
            return {f(unwrap())};
        }
        return {NONE};
    }

    ALWAYS_INLINE constexpr std::partial_ordering operator<=>(Opt const &other) const {
        if constexpr (Meta::Comparable<T>)
            if (_present and other._present)
                return _value <=> other._value;
        return std::partial_ordering::unordered;
    }

    ALWAYS_INLINE constexpr bool operator==(Opt const &other) const {
        if constexpr (Meta::Equatable<T>)
            if (_present and other._present)
                return _value == other._value;
        return false;
    }
};

static_assert(Tryable<Opt<isize>>);

} // namespace Karm
