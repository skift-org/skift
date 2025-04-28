#pragma once

#include <karm-meta/callable.h>
#include <karm-meta/traits.h>

#include "opt.h"
#include "panic.h"
#include "try.h"

namespace Karm {

template <typename T>
struct [[nodiscard]] Opt {
    struct _Empty {};

    bool _present;

    union {
        _Empty _empty;
        T _value;
    };

    always_inline constexpr Opt()
        : _present(false), _empty() {}

    always_inline constexpr Opt(None)
        : _present(false), _empty() {}

    template <typename U = T>
    always_inline constexpr Opt(U const& value)
        requires(not Meta::Same<Meta::RemoveConstVolatileRef<U>, Opt<T>> and Meta::CopyConstructible<T, U>)
        : _present(true), _value(value) {}

    template <typename U = T>
    always_inline constexpr Opt(U&& value)
        requires(not Meta::Same<Meta::RemoveConstVolatileRef<U>, Opt<T>> and Meta::MoveConstructible<T, U>)
        : _present(true), _value(std::forward<U>(value)) {}

    always_inline constexpr Opt(Opt const& other)
        requires(Meta::CopyConstructible<T>)
        : _present(other._present) {
        if (_present)
            std::construct_at(&_value, other.unwrap());
    }

    always_inline constexpr Opt(Opt&& other)
        requires(Meta::MoveConstructible<T>)
        : _present(other._present) {
        if (_present)
            std::construct_at(&_value, other.take());
    }

    template <typename U>
    always_inline constexpr Opt(Opt<U> const& other)
        requires(Meta::CopyConstructible<T, U>)
        : _present(other._present) {
        if (_present)
            std::construct_at(&_value, other.unwrap());
    }

    template <typename U>
        requires(Meta::MoveConstructible<T, U>)
    always_inline constexpr Opt(Opt<U>&& other)
        : _present(other._present) {
        if (_present)
            std::construct_at(&_value, other.take());
    }

    always_inline constexpr ~Opt() {
        clear();
    }

    always_inline constexpr Opt& operator=(None) {
        clear();
        return *this;
    }

    template <typename U = T>
        requires(not Meta::Same<Meta::RemoveConstVolatileRef<U>, Opt<T>> and Meta::Convertible<U, T>)
    always_inline constexpr Opt& operator=(U const& value) {
        clear();
        _present = true;
        std::construct_at(&_value, value);
        return *this;
    }

    template <typename U = T>
        requires(not Meta::Same<Meta::RemoveConstVolatileRef<U>, Opt<T>> and Meta::MoveConstructible<T, U>)
    always_inline constexpr Opt& operator=(U&& value) {
        clear();
        _present = true;
        std::construct_at(&_value, std::move(value));
        return *this;
    }

    always_inline constexpr Opt& operator=(Opt const& other)
        requires(Meta::CopyConstructible<T>)
    {
        *this = Opt(other);
        return *this;
    }

    always_inline constexpr Opt& operator=(Opt&& other)
        requires(Meta::MoveConstructible<T>)
    {
        clear();
        if (other._present) {
            _present = true;
            std::construct_at(&_value, other.take());
        }
        return *this;
    }

    template <typename U = T>
    always_inline constexpr Opt& operator=(Opt<U> const& other)
        requires(Meta::CopyConstructible<T, U>)
    {
        *this = Opt(other);
        return *this;
    }

    template <typename U>
    always_inline constexpr Opt& operator=(Opt<U>&& other)
        requires(Meta::MoveConstructible<T, U>)
    {
        clear();
        if (other._present) {
            _present = true;
            std::construct_at<T>(&_value, other.take());
        }
        return *this;
    }

    always_inline constexpr explicit operator bool() const {
        return _present;
    }

    always_inline constexpr bool has() const {
        return _present;
    }

    always_inline constexpr T* operator->() lifetimebound {
        if (not _present) [[unlikely]]
            panic("unwrapping None");

        return &_value;
    }

    always_inline constexpr T& operator*() lifetimebound {
        if (not _present) [[unlikely]]
            panic("unwrapping None");

        return _value;
    }

    always_inline constexpr T const* operator->() const lifetimebound {
        if (not _present) [[unlikely]]
            panic("unwrapping None");

        return &_value;
    }

    always_inline constexpr T const& operator*() const lifetimebound {
        if (not _present) [[unlikely]]
            panic("unwrapping None");

        return _value;
    }

    template <typename... Args>
    always_inline constexpr T& emplace(Args&&... args) lifetimebound {
        clear();
        _present = true;
        std::construct_at(&_value, std::forward<Args>(args)...);
        return _value;
    }

    always_inline constexpr void clear() {
        if (_present) {
            _value.~T();
            _present = false;
        }
    }

    always_inline constexpr None none() const {
        return NONE;
    }

    always_inline constexpr T& unwrap(char const* msg = "unwraping none") lifetimebound {
        if (not _present) [[unlikely]]
            panic(msg);
        return _value;
    }

    always_inline constexpr T const& unwrap(char const* msg = "unwraping none") const lifetimebound {
        if (not _present) [[unlikely]]
            panic(msg);
        return _value;
    }

    always_inline constexpr T const& unwrapOr(T const& other) const lifetimebound {
        if (_present)
            return _value;
        return other;
    }

    always_inline constexpr T unwrapOrDefault(T other) const lifetimebound {
        if (_present)
            return _value;
        return other;
    }

    always_inline constexpr T unwrapOrElse(auto f) const lifetimebound {
        if (_present)
            return _value;
        return f();
    }

    [[clang::coro_wrapper]]
    always_inline constexpr T take(char const* msg = "unwraping none") {
        if (not _present) [[unlikely]]
            panic(msg);
        T v = std::move(_value);
        clear();
        return v;
    }

    always_inline constexpr auto visit(auto visitor)
        -> decltype(visitor(_value)) {
        if (_present)
            return visitor(_value);
        return {};
    }

    always_inline constexpr auto visit(auto visitor) const
        -> decltype(visitor(_value)) {
        if (_present)
            return visitor(_value);
        return {};
    }

    // call operator
    template <typename... Args>
    always_inline constexpr auto operator()(Args&&... args) {
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

    // call operator
    template <typename... Args>
    always_inline constexpr auto operator()(Args&&... args) const {
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

    always_inline constexpr auto map(auto f) -> Opt<decltype(f(unwrap()))> {
        if (_present) {
            return {f(unwrap())};
        }
        return {NONE};
    }

    always_inline constexpr bool operator==(None) const {
        return not _present;
    }

    template <typename U>
        requires Meta::Equatable<T, U>
    always_inline constexpr bool operator==(U const& other) const {
        if (_present)
            return _value == other;
        return false;
    }

    template <typename U>
        requires Meta::Comparable<T, U>
    always_inline constexpr std::partial_ordering operator<=>(U const& other) const {
        if (_present)
            return _value <=> other;
        return std::partial_ordering::unordered;
    }

    always_inline constexpr bool operator==(Opt const& other) const {
        if constexpr (Meta::Equatable<T>)
            if (_present and other._present)
                return _value == other._value;
        return not _present and not other._present;
    }

    always_inline constexpr std::partial_ordering operator<=>(Opt const& other) const {
        if constexpr (Meta::Comparable<T>)
            if (_present and other._present)
                return _value <=> other._value;
        return std::partial_ordering::unordered;
    }
};

template <typename T>
struct Niche;

template <typename T>
concept Nicheable = requires {
    typename Niche<T>::Content;
};

template <Nicheable T>
struct [[nodiscard]] Opt<T> {
    union {
        T _value;
        Niche<T>::Content _content;
    };

    static_assert(sizeof(T) >= sizeof(typename Niche<T>::Content));

    always_inline constexpr Opt()
        : _content() {
    }

    always_inline constexpr Opt(None)
        : _content() {}

    template <typename U = T>
    always_inline constexpr Opt(U const& value)
        requires(not Meta::Same<Meta::RemoveConstVolatileRef<U>, Opt<T>> and Meta::CopyConstructible<T, U>)
        : _value(value) {
        _content.setupValue();
    }

    template <typename U = T>
    always_inline constexpr Opt(U&& value)
        requires(not Meta::Same<Meta::RemoveConstVolatileRef<U>, Opt<T>> and Meta::MoveConstructible<T, U>)
        : _value(std::forward<U>(value)) {
        _content.setupValue();
    }

    always_inline constexpr Opt(Opt const& other)
        requires(Meta::CopyConstructible<T>)
    {
        if (other.has()) {
            std::construct_at(&_value, other.unwrap());
            _content.setupValue();
        } else
            std::construct_at(&_content);
    }

    always_inline constexpr Opt(Opt&& other)
        requires(Meta::MoveConstructible<T>)
    {
        if (other.has()) {
            std::construct_at(&_value, other.take());
            _content.setupValue();
        } else
            std::construct_at(&_content);
    }

    template <typename U>
    always_inline constexpr Opt(Opt<U> const& other)
        requires(Meta::CopyConstructible<T, U>)
    {
        if (other.has()) {
            std::construct_at(&_value, other.unwrap());
            _content.setupValue();
        } else
            std::construct_at(&_content);
    }

    template <typename U>
        requires(Meta::MoveConstructible<T, U>)
    always_inline constexpr Opt(Opt<U>&& other) {
        if (other.has()) {
            std::construct_at(&_value, other.take());
            _content.setupValue();
        } else
            std::construct_at(&_content);
    }

    always_inline constexpr ~Opt() {
        clear();
    }

    always_inline constexpr Opt& operator=(None) {
        clear();
        return *this;
    }

    template <typename U = T>
        requires(not Meta::Same<Meta::RemoveConstVolatileRef<U>, Opt<T>> and Meta::Convertible<U, T>)
    always_inline constexpr Opt& operator=(U const& value) {
        clear();
        std::construct_at(&_value, value);
        _content.setupValue();
        return *this;
    }

    template <typename U = T>
        requires(not Meta::Same<Meta::RemoveConstVolatileRef<U>, Opt<T>> and Meta::MoveConstructible<T, U>)
    always_inline constexpr Opt& operator=(U&& value) {
        clear();
        std::construct_at(&_value, std::move(value));
        _content.setupValue();
        return *this;
    }

    always_inline constexpr Opt& operator=(Opt const& other)
        requires(Meta::CopyConstructible<T>)
    {
        *this = Opt(other);
        return *this;
    }

    always_inline constexpr Opt& operator=(Opt&& other)
        requires(Meta::MoveConstructible<T>)
    {
        clear();
        if (other.has()) {
            std::construct_at(&_value, other.take());
            _content.setupValue();
        }
        return *this;
    }

    template <typename U = T>
    always_inline constexpr Opt& operator=(Opt<U> const& other)
        requires(Meta::CopyConstructible<T, U>)
    {
        *this = Opt(other);
        return *this;
    }

    template <typename U>
    always_inline constexpr Opt& operator=(Opt<U>&& other)
        requires(Meta::MoveConstructible<T, U>)
    {
        clear();
        if (other.has()) {
            std::construct_at(&_value, other.take());
            _content.setupValue();
        }
        return *this;
    }

    always_inline constexpr explicit operator bool() const {
        return has();
    }

    always_inline constexpr bool has() const {
        return _content.has();
    }

    always_inline constexpr T* operator->() lifetimebound {
        if (not has()) [[unlikely]]
            panic("unwrapping None");

        return &_value;
    }

    always_inline constexpr T& operator*() lifetimebound {
        if (not has()) [[unlikely]]
            panic("unwrapping None");

        return _value;
    }

    always_inline constexpr T const* operator->() const lifetimebound {
        if (not has()) [[unlikely]]
            panic("unwrapping None");

        return &_value;
    }

    always_inline constexpr T const& operator*() const lifetimebound {
        if (not has()) [[unlikely]]
            panic("unwrapping None");

        return _value;
    }

    template <typename... Args>
    always_inline constexpr T& emplace(Args&&... args) lifetimebound {
        clear();
        std::construct_at(&_value, std::forward<Args>(args)...);
        return _value;
    }

    always_inline constexpr void clear() {
        if (has()) {
            _value.~T();
            std::construct_at(&_content);
        }
    }

    always_inline constexpr None none() const {
        return NONE;
    }

    always_inline constexpr T& unwrap(char const* msg = "unwraping none") lifetimebound {
        if (not has()) [[unlikely]]
            panic(msg);
        return _value;
    }

    always_inline constexpr T const& unwrap(char const* msg = "unwraping none") const lifetimebound {
        if (not has()) [[unlikely]]
            panic(msg);
        return _value;
    }

    always_inline constexpr T const& unwrapOr(T const& other) const lifetimebound {
        if (has())
            return _value;
        return other;
    }

    always_inline constexpr T unwrapOrDefault(T other) const lifetimebound {
        if (has())
            return _value;
        return other;
    }

    always_inline constexpr T unwrapOrElse(auto f) const lifetimebound {
        if (has())
            return _value;
        return f();
    }

    always_inline constexpr T take(char const* msg = "unwraping none") {
        if (not has()) [[unlikely]]
            panic(msg);
        T v = std::move(_value);
        clear();
        return v;
    }

    always_inline constexpr auto visit(auto visitor)
        -> decltype(visitor(_value)) {
        if (has())
            return visitor(_value);
        return {};
    }

    always_inline constexpr auto visit(auto visitor) const
        -> decltype(visitor(_value)) {
        if (has())
            return visitor(_value);
        return {};
    }

    // call operator
    template <typename... Args>
    always_inline constexpr auto operator()(Args&&... args) {
        using OptRet = Opt<Meta::Ret<T, Args...>>;

        if constexpr (Meta::Same<void, Meta::Ret<T, Args...>>) {
            // Handle void return type
            if (not has()) {
                return false;
            }
            _value(std::forward<Args>(args)...);
            return true;
        } else {
            // Handle non-void return type
            if (not has()) {
                return OptRet{NONE};
            }

            return OptRet{_value(std::forward<Args>(args)...)};
        }
    }

    // call operator
    template <typename... Args>
    always_inline constexpr auto operator()(Args&&... args) const {
        using OptRet = Opt<Meta::Ret<T, Args...>>;

        if constexpr (Meta::Same<void, Meta::Ret<T, Args...>>) {
            // Handle void return type
            if (not has()) {
                return false;
            }
            _value(std::forward<Args>(args)...);
            return true;
        } else {
            // Handle non-void return type
            if (not has()) {
                return OptRet{NONE};
            }

            return OptRet{_value(std::forward<Args>(args)...)};
        }
    }

    always_inline constexpr auto map(auto f) -> Opt<decltype(f(unwrap()))> {
        if (has()) {
            return {f(unwrap())};
        }
        return {NONE};
    }

    always_inline constexpr bool operator==(None) const {
        return not has();
    }

    template <typename U>
        requires Meta::Equatable<T, U>
    always_inline constexpr bool operator==(U const& other) const {
        if (has())
            return _value == other;
        return false;
    }

    template <typename U>
        requires Meta::Comparable<T, U>
    always_inline constexpr std::partial_ordering operator<=>(U const& other) const {
        if (has())
            return _value <=> other;
        return std::partial_ordering::unordered;
    }

    always_inline constexpr bool operator==(Opt const& other) const {
        if constexpr (Meta::Equatable<T>)
            if (has() and other.has())
                return _value == other._value;
        return not has() and not other.has();
    }

    always_inline constexpr std::partial_ordering operator<=>(Opt const& other) const {
        if constexpr (Meta::Comparable<T>)
            if (has() and other.has())
                return _value <=> other._value;
        return std::partial_ordering::unordered;
    }
};

template <>
struct Niche<bool> {
    struct Content {
        u8 data;

        constexpr Content() : data(2) {}

        constexpr bool has() const {
            return data != 2;
        }

        constexpr void setupValue() {}
    };
};

template <Meta::Enum T>
    requires requires { T::_LEN; }
struct Niche<T> {
    struct Content {
        using IntType = Meta::UnderlyingType<T>;
        IntType data;

        constexpr Content() : data(IntType(T::_LEN) + 1) {}

        constexpr bool has() const {
            return data != (IntType(T::_LEN) + 1);
        }

        constexpr void setupValue() {}
    };
};

inline constexpr char _NONE_VALUE = 'e';
inline char const* _NONE_PTR = &_NONE_VALUE;

static_assert(Tryable<Opt<isize>>);

} // namespace Karm
