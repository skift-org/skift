#pragma once

#include <karm-meta/traits.h>

#include "base.h"
#include "string.h"

namespace Karm {

template <Meta::Enum E>
struct Flags {
    E _value = {};

    Flags() = default;

    Flags(E value)
        : _value(value) {}

    bool has(E value) const {
        return (_value & value) == value;
    }

    void set(E value) {
        _value |= value;
    }

    void unset(E value) {
        _value &= ~value;
    }

    void toggle(E value) {
        _value ^= value;
    }

    void clear() {
        _value = {};
    }

    bool empty() const {
        return _value == (E)0;
    }

    bool any() const {
        return _value != (E)0;
    }

    E val() const {
        return _value;
    }

    auto underlying() const {
        return (Meta::UnderlyingType<E>)_value;
    }

    operator E() const {
        return _value;
    }

    explicit operator bool() const {
        return _value != (E)0;
    }

    Flags operator~() const {
        return Flags(~_value);
    }

    Flags operator|(Flags other) const {
        return Flags(_value | other._value);
    }

    Flags operator&(Flags other) const {
        return Flags(_value & other._value);
    }

    Flags operator^(Flags other) const {
        return Flags(_value ^ other._value);
    }

    Flags &operator|=(Flags other) {
        _value |= other._value;
        return *this;
    }

    Flags &operator&=(Flags other) {
        _value &= other._value;
        return *this;
    }

    Flags &operator^=(Flags other) {
        _value ^= other._value;
        return *this;
    }

    bool operator!() const {
        return !_value;
    }

    bool operator==(Flags other) const {
        return _value == other._value;
    }

    bool operator==(E other) const {
        return _value == other;
    }

    bool operator!=(Flags other) const {
        return _value != other._value;
    }

    bool operator!=(E other) const {
        return _value != other;
    }

    bool operator<(Flags other) const {
        return _value < other._value;
    }

    bool operator>(Flags other) const {
        return _value > other._value;
    }

    bool operator<=(Flags other) const {
        return _value <= other._value;
    }

    bool operator>=(Flags other) const {
        return _value >= other._value;
    }
};

template <Meta::Enum E, typename U = Meta::UnderlyingType<E>>
constexpr U toUnderlyingType(E value) {
    return (U)value;
};

// MARK: Utilities -------------------------------------------------------------

// HACK: Sometime they can be multiple enums in the same scope
//       This allow the _LEN to not conflict
#define FOREACH_ENUM_LEN(LEN) \
    LEN()                     \
    LEN(0)                    \
    LEN(1)                    \
    LEN(2)                    \
    LEN(3)                    \
    LEN(4)                    \
    LEN(5)                    \
    LEN(6)                    \
    LEN(7)

template <typename E>
concept BoundedEnum =
    Meta::Enum<E> and
    (requires { E::_MAX; } or
     (
#define ITER(LEN) requires { E::_LEN##LEN; } or
         FOREACH_ENUM_LEN(ITER)
#undef ITER
             false
     ));

template <Meta::Enum E>
consteval Meta::UnderlyingType<E> enumMin() {
    if constexpr (requires { E::_MIN; }) {
        return toUnderlyingType(E::_MIN);
    }
#define ITER(LEN)                                    \
    else if constexpr (requires { E::_LEN##LEN; }) { \
        return Meta::UnderlyingType<E>{};            \
    }
    FOREACH_ENUM_LEN(ITER)
#undef ITER
    else {
        static_assert(false, "Enum must have _MIN or _LEN");
    }
}

template <Meta::Enum E>
consteval Meta::UnderlyingType<E> enumMax() {
    if constexpr (requires { E::_MAX; }) {
        return toUnderlyingType(E::_MAX);
    }
#define ITER(LEN)                                    \
    else if constexpr (requires { E::_LEN##LEN; }) { \
        return toUnderlyingType(E::_LEN##LEN) - 1;   \
    }
    FOREACH_ENUM_LEN(ITER)
#undef ITER
    else {
        static_assert(false, "Enum must have _MAX or _LEN");
    }
}

template <Meta::Enum E>
consteval Meta::UnderlyingType<E> enumLen() {
#define ITER(LEN)                               \
    if constexpr (requires { E::_LEN##LEN; }) { \
        return toUnderlyingType(E::_LEN##LEN);  \
    } else
    FOREACH_ENUM_LEN(ITER)
#undef ITER
    return enumMax<E>() - enumMin<E>() + 1;
}

template <typename T>
static constexpr Str nameOf() {
    Str v = __PRETTY_FUNCTION__;
    auto start = indexOf(v, '=').take();
    start += 2;
    return sub(v, start, v.len() - 1);
}

template <Meta::Enum E, E V>
static constexpr Str nameOf() {
    Str v = __PRETTY_FUNCTION__;
    auto start = lastIndexOf(v, ':').take();
    start += 1;
    return sub(v, start, v.len() - 1);
}

template <Meta::Enum E>
struct _EnumItem {
    Str name;
    Meta::UnderlyingType<E> value;
};

template <Meta::Enum E, E V>
consteval auto _enumItem() {
    return _EnumItem<E>{nameOf<E, V>(), toUnderlyingType(V)};
}

template <Meta::Enum E, Meta::UnderlyingType<E>... Vs>
static constexpr Array<_EnumItem<E>, sizeof...(Vs)> _enumItems(std::integer_sequence<Meta::UnderlyingType<E>, Vs...>) {
    return {_enumItem<E, E(Vs + enumMin<E>())>()...};
}

template <Meta::Enum E>
static constexpr auto enumItems() {
    return _enumItems<E>(std::make_integer_sequence<Meta::UnderlyingType<E>, enumLen<E>()>{});
}

template <Meta::Enum E>
static constexpr Str nameOf(E v) {
    auto items = enumItems<E>();
    for (auto &i : items) {
        if (i.value == toUnderlyingType(v))
            return i.name;
    }
    return "<unknown>";
}

template <Meta::Enum E>
static constexpr Opt<E> valueOf(Str name) {
    auto items = enumItems<E>();
    for (auto &i : items) {
        if (i.name == name)
            return E(i.value);
    }
    return NONE;
}

} // namespace Karm
