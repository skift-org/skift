#pragma once

#include "enum.h"
#include "string.h"
#include "tuple.h"

namespace Karm {

template <typename T>
static constexpr Str nameOf() {
    Str v = __PRETTY_FUNCTION__;
    auto start = indexOf(v, '=').take();
    start += 2;
    return sub(v, start, v.len() - 1);
}

template <typename T>
struct Reflect {
    using Type = T;
    static constexpr Str NAME = nameOf<T>();
};

template <typename T, typename FT, StrLit N, FT T::*P>
struct _Field {
    using Type = FT;
    using Reflect = Reflect<FT>;

    static constexpr auto _STORE = N;
    static constexpr auto NAME = Str(_STORE);
    static constexpr auto PTR = P;

    static auto &access(T &t) {
        return t.*PTR;
    }

    static auto const &access(T const &t) {
        return t.*PTR;
    }
};

template <typename T>
concept Reflectable = requires() {
    typename Reflect<T>::Fields;
};

template <typename T>
auto iterFields(T &v, auto f) {
    using Fs = Reflect<Meta::RemoveConstVolatileRef<T>>::Fields;
    return Fs::inspect([&]<typename F>(Meta::Type<F>) {
        return f(F::NAME, F::access(v));
    });
}

// MARK: Enums -----------------------------------------------------------------

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

template <Meta::Enum E>
consteval Meta::UnderlyingType<E> _enumMin() {
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
consteval Meta::UnderlyingType<E> _enumMax() {
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
consteval Meta::UnderlyingType<E> _enumLen() {
#define ITER(LEN)                               \
    if constexpr (requires { E::_LEN##LEN; }) { \
        return toUnderlyingType(E::_LEN##LEN);  \
    } else
    FOREACH_ENUM_LEN(ITER)
#undef ITER
    return _enumMax<E>() - _enumMin<E>() + 1;
}

template <Meta::Enum E, E V>
static constexpr Str nameOf() {
    Str v = __PRETTY_FUNCTION__;
    auto start = lastIndexOf(v, ':').take();
    start += 1;
    return sub(v, start, v.len() - 1);
}

template <Meta::Enum E>
struct _EnumField {
    Str name;
    Meta::UnderlyingType<E> value;
};

template <Meta::Enum E, E V>
consteval auto _makeEnumField() {
    return _EnumField<E>{nameOf<E, V>(), toUnderlyingType(V)};
}

template <Meta::Enum E, Meta::UnderlyingType<E>... Vs>
static constexpr Array<_EnumField<E>, sizeof...(Vs)> _makeEnumFields(std::integer_sequence<Meta::UnderlyingType<E>, Vs...>) {
    return {_makeEnumField<E, E(Vs + _enumMin<E>())>()...};
}

template <typename E>
concept ReflectableEnum =
    Meta::Enum<E> and
    (requires { E::_MAX; } or
     (
#define ITER(LEN) requires { E::_LEN##LEN; } or
         FOREACH_ENUM_LEN(ITER)
#undef ITER
             false
     ));

template <ReflectableEnum E>
struct Reflect<E> {
    using Type = E;
    static constexpr Str NAME = nameOf<E>();

    static constexpr Meta::UnderlyingType<E> MIN = _enumMin<E>();
    static constexpr Meta::UnderlyingType<E> MAX = _enumMax<E>();
    static constexpr Meta::UnderlyingType<E> LEN = _enumLen<E>();
    static constexpr auto FIELDS = _makeEnumFields<E>(std::make_integer_sequence<Meta::UnderlyingType<E>, _enumLen<E>()>{});
};

template <Meta::Enum E>
static constexpr Str nameOf(E v) {
    using R = Reflect<E>;
    for (auto &f : R::FIELDS) {
        if (f.value == toUnderlyingType(v))
            return f.name;
    }
    return "<unknown>";
}

template <Meta::Enum E>
static constexpr Opt<E> valueOf(Str name) {
    using R = Reflect<E>;
    for (auto &f : R::FIELDS) {
        if (f.name == name)
            return E(f.value);
    }
    return {};
}

// MARK: Utilities -------------------------------------------------------------

} // namespace Karm

#define __reflectableField(F) \
    ::Karm::_Field<Type, decltype(Meta::declval<Type>().F), #F, &Type::F>

#define __reflectableFields(...) \
    ::Karm::Tuple<mapList$(__reflectableField, __VA_ARGS__)>

#define __reflectableType(T, ...)                        \
    struct ::Karm::Reflect<T> {                          \
        using Type = T;                                  \
        static constexpr Str NAME = ::Karm::nameOf<T>(); \
        using Fields = __reflectableFields(__VA_ARGS__); \
    }

#define Reflectable$(T, ...) \
    template <>              \
    __reflectableType(T __VA_OPT__(, ) __VA_ARGS__)

#define ReflectableTemplate$(T, ...) \
    __reflectableType(T __VA_OPT__(, ) __VA_ARGS__)

template <typename T>
ReflectableTemplate$(Karm::Range<T>, start, size);

#undef FOREACH_ENUM_LEN
