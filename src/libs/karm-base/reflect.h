#pragma once

#include <karm-base/macros.h>
#include <karm-base/string.h>
#include <karm-base/tuple.h>

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
    static constexpr Str NAME = Karm::nameOf<T>();
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

} // namespace Karm

#define __reflectableField(F) \
    Karm::_Field<Type, decltype(Meta::declval<Type>().F), #F, &Type::F>

#define __reflectableFields(...) \
    Karm::Tuple<mapList$(__reflectableField, __VA_ARGS__)>

#define __reflectableType(T, ...)                        \
    struct Karm::Reflect<T> {                            \
        using Type = T;                                  \
        static constexpr Str NAME = Karm::nameOf<T>();   \
        using Fields = __reflectableFields(__VA_ARGS__); \
    }

#define Reflectable$(T, ...) \
    template <>              \
    __reflectableType(T __VA_OPT__(, ) __VA_ARGS__)

#define ReflectableTemplate$(T, ...) \
    __reflectableType(T __VA_OPT__(, ) __VA_ARGS__)
