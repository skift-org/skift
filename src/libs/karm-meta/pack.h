#pragma once

#include <karm-base/base.h>

#include "cvrp.h"
#include "id.h"
#include "traits.h"

namespace Karm::Meta {

template <typename T, typename... Ts>
concept Contains = (Same<T, Ts> or ...);

template <usize Start, typename T, typename... Ts>
    requires(Contains<T, Ts...>)
consteval usize _indexOf() {
    if constexpr (Same<T, Ts...[Start]>)
        return Start;
    else
        return _indexOf<Start + 1, T, Ts...>();
}

template <typename T, typename... Ts>
    requires(Contains<T, Ts...>)
consteval usize indexOf() {
    return _indexOf<0, T, Ts...>();
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

template <typename T, typename... Ts>
struct _Any {
    always_inline static auto eval(auto func)
        requires(not Meta::Same<decltype(func.template operator()<T>()), void>)
    {
        auto test = func.template operator()<T>();
        test or ((test = func.template operator()<Ts>()) or ...);
        return test;
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
    if constexpr (sizeof...(Ts) == 0)
        return;
    else
        return _Any<Ts...>::eval(func);
}

// MARK: First -----------------------------------------------------------------

template <typename...>
struct _First;

template <typename T, typename... Ts>
struct _First<T, Ts...> {
    using Type = T;
};

template <typename... Ts>
using First = typename _First<Ts...>::Type;

} // namespace Karm::Meta
