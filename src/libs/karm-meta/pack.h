#pragma once

#include <karm-base/macros.h>
#include <karm-base/std.h>

#include "cvrp.h"
#include "traits.h"

namespace Karm::Meta {

template <typename T, typename... List>
concept Contains = (Same<T, List> or ...);

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
    ALWAYS_INLINE static auto eval(usize, Data *ptr, auto func) {
        using U = CopyConst<Data, T>;
        return func(*reinterpret_cast<U *>(ptr));
    }
};

template <typename Data, typename First, typename... Rest>
struct _IndexCast<Data, First, Rest...> {
    ALWAYS_INLINE static auto eval(usize index, Data *ptr, auto func) {
        using U = CopyConst<Data, First>;

        return index == 0 ? func(*reinterpret_cast<U *>(ptr))
                          : _IndexCast<Data, Rest...>::eval(index - 1, ptr, func);
    }
};

template <typename... Ts>
ALWAYS_INLINE static auto indexCast(usize index, auto *ptr, auto func) {
    return _IndexCast<RemoveRef<decltype(*ptr)>, Ts...>::eval(index, ptr, func);
}

} // namespace Karm::Meta
