#pragma once

#include <karm-base/std.h>

#include "same.h"
#include "const.h"
#include "ref.h"

namespace Karm::Meta {

template <typename T, typename... List>
concept Contains = (Same<T, List> || ...);

template <typename T, typename... Ts>
struct _IndexOf;

template <typename T, typename First>
struct _IndexOf<T, First> : Value<size_t, 0> {
};

template <typename T, typename First, typename... Rest>
struct _IndexOf<T, First, Rest...> : Value<size_t, Same<T, First> ? 0 : _IndexOf<T, Rest...>() + 1> {
};

template <typename T, typename... Ts>
static consteval size_t index_of() {
    return _IndexOf<T, Ts...>::VALUE;
}

template <typename... Ts>
struct _IndexCast;

template <typename Data, typename T>
struct _IndexCast<Data, T> {
    static void eval(size_t, Data *ptr, auto func) {
        using U = CopyConst<Data, T>;
        func(*reinterpret_cast<U *>(ptr));
    }
};

template <typename Data, typename First, typename... Rest>
struct _IndexCast<Data, First, Rest...> {
    static void eval(size_t index, Data *ptr, auto func) {
        using U = CopyConst<Data, First>;

        index == 0 ? func(*reinterpret_cast<U *>(ptr))
                   : _IndexCast<Data, Rest...>::eval(index - 1, ptr, func);
    }
};

template <typename... Ts>
static void index_cast(size_t index, auto *ptr, auto func) {
    _IndexCast<RemoveRef<decltype(*ptr)>, Ts...>::eval(index, ptr, func);
}

} // namespace Karm::Meta
