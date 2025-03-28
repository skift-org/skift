#pragma once

#include <karm-base/base.h>

#include "cvrp.h"
#include "id.h"
#include "traits.h"

namespace Karm::Meta {

template <typename T, typename... Ts>
concept Contains = (Same<T, Ts> or ...);

template <typename T, typename... Ts>
struct _IndexOf;

template <typename T, Meta::Same<T> First>
struct _IndexOf<T, First> {
    static constexpr usize value = 0;
};

template <typename T, typename First, typename... Rest>
struct _IndexOf<T, First, Rest...> {
    static constexpr usize value = [] {
        if constexpr (Same<T, First>)
            return 0;
        else
            return 1 + _IndexOf<T, Rest...>::value;
    }();
};

template <typename T, typename... Ts>
    requires(Contains<T, Ts...>)
consteval usize indexOf() {
    return _IndexOf<T, Ts...>::value;
}

template <typename...>
struct _IndexCast;

template <typename Data, typename T>
struct _IndexCast<Data, T> {
    always_inline static auto eval(usize, Data* ptr, auto func) {
        using U = CopyConst<Data, T>;
        return func(*reinterpret_cast<U*>(ptr));
    }
};

template <typename Data, typename T, typename... Ts>
struct _IndexCast<Data, T, Ts...> {
    always_inline static auto eval(usize index, Data* ptr, auto func) {
        using U = CopyConst<Data, T>;

        return index == 0 ? func(*reinterpret_cast<U*>(ptr))
                          : _IndexCast<Data, Ts...>::eval(index - 1, ptr, func);
    }
};

template <typename... Ts>
always_inline static auto indexCast(usize index, auto* ptr, auto func) {
    return _IndexCast<RemoveRef<decltype(*ptr)>, Ts...>::eval(index, ptr, func);
}

template <typename...>
struct _Any;

template <typename T>
struct _Any<T> {
    always_inline static auto eval(auto func) {
        return func.template operator()<T>();
    }
};

template <typename T, typename... Ts>
struct _Any<T, Ts...> {
    always_inline static auto eval(auto func)
        requires(not Meta::Same<decltype(func.template operator()<T>()), void>)
    {
        return func.template operator()<T>() ?: _Any<Ts...>::eval(func);
    }

    always_inline static auto eval(auto func)
        requires(Meta::Same<decltype(func.template operator()<T>()), void>)
    {
        func.template operator()<T>();
        (func.template operator()<Ts>(), ...);
    }
};

template <typename... Ts>
always_inline static auto any(auto func) {
    return _Any<Ts...>::eval(func);
}

// MARK: First -----------------------------------------------------------------

template <typename...>
struct _First;

template <typename T>
struct _First<T> {
    using Type = T;
};

template <typename T, typename... Ts>
struct _First<T, Ts...> {
    using Type = T;
};

template <typename... Ts>
using First = typename _First<Ts...>::Type;

} // namespace Karm::Meta
