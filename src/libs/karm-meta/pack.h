#pragma once

#include <karm-base/base.h>

#include "cvrp.h"
#include "id.h"
#include "traits.h"

namespace Karm::Meta {

template <typename T, typename... Ts>
concept Contains = (Same<T, Ts> or ...);

template <typename...>
inline constexpr usize _indexOf = 0;

template <typename T, typename First>
inline constexpr usize _indexOf<T, First> = 0;

template <typename T, typename First, typename... Rest>
inline constexpr usize _indexOf<T, First, Rest...> = Same<T, First> ? 0 : _indexOf<T, Rest...> + 1;

template <typename T, typename... Ts>
    requires Contains<T, Ts...>
static consteval usize indexOf() {
    return _indexOf<T, Ts...>;
}

template <typename...>
struct _IndexCast;

template <typename Data, typename T>
struct _IndexCast<Data, T> {
    always_inline static auto eval(usize, Data *ptr, auto func) {
        using U = CopyConst<Data, T>;
        return func(*reinterpret_cast<U *>(ptr));
    }
};

template <typename Data, typename T, typename... Ts>
struct _IndexCast<Data, T, Ts...> {
    always_inline static auto eval(usize index, Data *ptr, auto func) {
        using U = CopyConst<Data, T>;

        return index == 0 ? func(*reinterpret_cast<U *>(ptr))
                          : _IndexCast<Data, Ts...>::eval(index - 1, ptr, func);
    }
};

template <typename... Ts>
always_inline static auto indexCast(usize index, auto *ptr, auto func) {
    return _IndexCast<RemoveRef<decltype(*ptr)>, Ts...>::eval(index, ptr, func);
}

template <typename...>
struct _Any;

template <typename T>
struct _Any<T> {
    always_inline static auto eval(auto func) {
        return func(Type<T>{});
    }
};

template <typename T, typename... Ts>
struct _Any<T, Ts...> {
    always_inline static auto eval(auto func)
        requires(not Meta::Same<decltype(func(Type<T>{})), void>)
    {
        return func(Type<T>{}) ?: _Any<Ts...>::eval(func);
    }

    always_inline static auto eval(auto func)
        requires(Meta::Same<decltype(func(Type<T>{})), void>)
    {
        func(Type<T>{});
        _Any<Ts...>::eval(func);
    }
};

template <typename... Ts>
always_inline static auto any(auto func) {
    return _Any<Ts...>::eval(func);
}

} // namespace Karm::Meta
