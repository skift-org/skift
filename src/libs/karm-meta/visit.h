#pragma once

#include <type_traits>

#include "cvrp.h"
#include "traits.h"

namespace Karm::Meta {

template <bool Cond>
struct _VisitForwardLike {
    template <class T>
    using type = Meta::RemoveRef<T> &&;
};

template <>
struct _VisitForwardLike<true> {
    template <class T>
    using type = Meta::RemoveRef<T> &;
};

#define __visitForward$(...) static_cast<decltype(__VA_ARGS__) &&>(__VA_ARGS__)
#define __visitForwardLike$(T, ...) static_cast<typename ::Karm::Meta::_VisitForwardLike<::Karm::Meta::LvalueRef<T>>::template type<decltype(__VA_ARGS__)>>(__VA_ARGS__)

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&, std::integral_constant<size_t, 0>) {
    return __visitForward$(fn)();
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 1>) {
    auto &&[_1] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 2>) {
    auto &&[_1, _2] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 3>) {
    auto &&[_1, _2, _3] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 4>) {
    auto &&[_1, _2, _3, _4] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 5>) {
    auto &&[_1, _2, _3, _4, _5] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 6>) {
    auto &&[_1, _2, _3, _4, _5, _6] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 7>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 8>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 9>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 10>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 11>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 12>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 13>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 14>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 15>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 16>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 17>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 18>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 19>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 20>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 21>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 22>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 23>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 24>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 25>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 26>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 27>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 28>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 29>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 30>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 31>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 32>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 33>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 34>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 35>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 36>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 37>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 38>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 39>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 40>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 41>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 42>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 43>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 44>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 45>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 46>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 47>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 48>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 49>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48), __visitForwardLike$(T, _49));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 50>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48), __visitForwardLike$(T, _49), __visitForwardLike$(T, _50));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 51>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48), __visitForwardLike$(T, _49), __visitForwardLike$(T, _50), __visitForwardLike$(T, _51));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 52>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48), __visitForwardLike$(T, _49), __visitForwardLike$(T, _50), __visitForwardLike$(T, _51), __visitForwardLike$(T, _52));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 53>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48), __visitForwardLike$(T, _49), __visitForwardLike$(T, _50), __visitForwardLike$(T, _51), __visitForwardLike$(T, _52), __visitForwardLike$(T, _53));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 54>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48), __visitForwardLike$(T, _49), __visitForwardLike$(T, _50), __visitForwardLike$(T, _51), __visitForwardLike$(T, _52), __visitForwardLike$(T, _53), __visitForwardLike$(T, _54));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 55>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48), __visitForwardLike$(T, _49), __visitForwardLike$(T, _50), __visitForwardLike$(T, _51), __visitForwardLike$(T, _52), __visitForwardLike$(T, _53), __visitForwardLike$(T, _54), __visitForwardLike$(T, _55));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 56>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48), __visitForwardLike$(T, _49), __visitForwardLike$(T, _50), __visitForwardLike$(T, _51), __visitForwardLike$(T, _52), __visitForwardLike$(T, _53), __visitForwardLike$(T, _54), __visitForwardLike$(T, _55), __visitForwardLike$(T, _56));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 57>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48), __visitForwardLike$(T, _49), __visitForwardLike$(T, _50), __visitForwardLike$(T, _51), __visitForwardLike$(T, _52), __visitForwardLike$(T, _53), __visitForwardLike$(T, _54), __visitForwardLike$(T, _55), __visitForwardLike$(T, _56), __visitForwardLike$(T, _57));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 58>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48), __visitForwardLike$(T, _49), __visitForwardLike$(T, _50), __visitForwardLike$(T, _51), __visitForwardLike$(T, _52), __visitForwardLike$(T, _53), __visitForwardLike$(T, _54), __visitForwardLike$(T, _55), __visitForwardLike$(T, _56), __visitForwardLike$(T, _57), __visitForwardLike$(T, _58));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 59>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48), __visitForwardLike$(T, _49), __visitForwardLike$(T, _50), __visitForwardLike$(T, _51), __visitForwardLike$(T, _52), __visitForwardLike$(T, _53), __visitForwardLike$(T, _54), __visitForwardLike$(T, _55), __visitForwardLike$(T, _56), __visitForwardLike$(T, _57), __visitForwardLike$(T, _58), __visitForwardLike$(T, _59));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 60>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48), __visitForwardLike$(T, _49), __visitForwardLike$(T, _50), __visitForwardLike$(T, _51), __visitForwardLike$(T, _52), __visitForwardLike$(T, _53), __visitForwardLike$(T, _54), __visitForwardLike$(T, _55), __visitForwardLike$(T, _56), __visitForwardLike$(T, _57), __visitForwardLike$(T, _58), __visitForwardLike$(T, _59), __visitForwardLike$(T, _60));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 61>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48), __visitForwardLike$(T, _49), __visitForwardLike$(T, _50), __visitForwardLike$(T, _51), __visitForwardLike$(T, _52), __visitForwardLike$(T, _53), __visitForwardLike$(T, _54), __visitForwardLike$(T, _55), __visitForwardLike$(T, _56), __visitForwardLike$(T, _57), __visitForwardLike$(T, _58), __visitForwardLike$(T, _59), __visitForwardLike$(T, _60), __visitForwardLike$(T, _61));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 62>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48), __visitForwardLike$(T, _49), __visitForwardLike$(T, _50), __visitForwardLike$(T, _51), __visitForwardLike$(T, _52), __visitForwardLike$(T, _53), __visitForwardLike$(T, _54), __visitForwardLike$(T, _55), __visitForwardLike$(T, _56), __visitForwardLike$(T, _57), __visitForwardLike$(T, _58), __visitForwardLike$(T, _59), __visitForwardLike$(T, _60), __visitForwardLike$(T, _61), __visitForwardLike$(T, _62));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 63>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48), __visitForwardLike$(T, _49), __visitForwardLike$(T, _50), __visitForwardLike$(T, _51), __visitForwardLike$(T, _52), __visitForwardLike$(T, _53), __visitForwardLike$(T, _54), __visitForwardLike$(T, _55), __visitForwardLike$(T, _56), __visitForwardLike$(T, _57), __visitForwardLike$(T, _58), __visitForwardLike$(T, _59), __visitForwardLike$(T, _60), __visitForwardLike$(T, _61), __visitForwardLike$(T, _62), __visitForwardLike$(T, _63));
}

template <class Fn, class T>
[[nodiscard]] constexpr decltype(auto) _visit(Fn &&fn, T &&t, std::integral_constant<size_t, 64>) {
    auto &&[_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64] = __visitForward$(t);
    return __visitForward$(fn)(__visitForwardLike$(T, _1), __visitForwardLike$(T, _2), __visitForwardLike$(T, _3), __visitForwardLike$(T, _4), __visitForwardLike$(T, _5), __visitForwardLike$(T, _6), __visitForwardLike$(T, _7), __visitForwardLike$(T, _8), __visitForwardLike$(T, _9), __visitForwardLike$(T, _10), __visitForwardLike$(T, _11), __visitForwardLike$(T, _12), __visitForwardLike$(T, _13), __visitForwardLike$(T, _14), __visitForwardLike$(T, _15), __visitForwardLike$(T, _16), __visitForwardLike$(T, _17), __visitForwardLike$(T, _18), __visitForwardLike$(T, _19), __visitForwardLike$(T, _20), __visitForwardLike$(T, _21), __visitForwardLike$(T, _22), __visitForwardLike$(T, _23), __visitForwardLike$(T, _24), __visitForwardLike$(T, _25), __visitForwardLike$(T, _26), __visitForwardLike$(T, _27), __visitForwardLike$(T, _28), __visitForwardLike$(T, _29), __visitForwardLike$(T, _30), __visitForwardLike$(T, _31), __visitForwardLike$(T, _32), __visitForwardLike$(T, _33), __visitForwardLike$(T, _34), __visitForwardLike$(T, _35), __visitForwardLike$(T, _36), __visitForwardLike$(T, _37), __visitForwardLike$(T, _38), __visitForwardLike$(T, _39), __visitForwardLike$(T, _40), __visitForwardLike$(T, _41), __visitForwardLike$(T, _42), __visitForwardLike$(T, _43), __visitForwardLike$(T, _44), __visitForwardLike$(T, _45), __visitForwardLike$(T, _46), __visitForwardLike$(T, _47), __visitForwardLike$(T, _48), __visitForwardLike$(T, _49), __visitForwardLike$(T, _50), __visitForwardLike$(T, _51), __visitForwardLike$(T, _52), __visitForwardLike$(T, _53), __visitForwardLike$(T, _54), __visitForwardLike$(T, _55), __visitForwardLike$(T, _56), __visitForwardLike$(T, _57), __visitForwardLike$(T, _58), __visitForwardLike$(T, _59), __visitForwardLike$(T, _60), __visitForwardLike$(T, _61), __visitForwardLike$(T, _62), __visitForwardLike$(T, _63), __visitForwardLike$(T, _64));
}

struct _VisitAny {
    template <class U>
    operator U() const;
};

template <class T>
struct _VisitExcept {
    template <class U>
        requires(not Meta::Derive<T, U>)
    operator U() const noexcept;
};

template <class T, size_t Bases = 0u, class... Ts>
    requires Meta::Agregate<T>
[[nodiscard]] constexpr auto size() -> size_t {
    if constexpr (requires { T{Ts{}...}; } and not requires { T{Ts{}..., _VisitAny{}}; }) {
        return sizeof...(Ts) - Bases;
    } else if constexpr (Bases == sizeof...(Ts) and requires { T{Ts{}...}; } and not requires { T{Ts{}..., _VisitExcept<T>{}}; }) {
        return size<T, Bases + 1u, Ts..., _VisitAny>();
    } else {
        return size<T, Bases, Ts..., _VisitAny>();
    }
}

template <class Fn, class T>
    requires Meta::Agregate<Meta::RemoveConstVolatileRef<T>>
[[nodiscard]] constexpr decltype(auto) visit(Fn &&fn, T &&t, auto...) {
    return _visit(__visitForward$(fn), __visitForward$(t), std::integral_constant<size_t, size<Meta::RemoveConstVolatileRef<T>>()>{});
}

} // namespace Karm::Meta
