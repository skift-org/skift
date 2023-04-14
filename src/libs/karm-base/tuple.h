#pragma once

#include <karm-meta/id.h>

namespace Karm {

template <usize I, typename T>
struct _T;

#define _TN(N)            \
    template <typename T> \
    struct _T<N, T> {     \
        using T##N = T;   \
    }

// clang-format off
_TN(0);  _TN(1);  _TN(2);  _TN(3);  _TN(4);  _TN(5);  _TN(6);  _TN(7);
_TN(8);  _TN(9); _TN(10); _TN(11); _TN(12); _TN(13); _TN(14); _TN(15);
// Do we need more than 16?
// clang-format on

template <usize I, typename T>
struct _V;

#define _VN(N)                                            \
    template <typename T>                                 \
    struct _V<N, T> : public _T<N, T> {                   \
        T v##N;                                           \
        constexpr _V(T &&v) : v##N{std::forward<T>(v)} {} \
        auto visit(auto f) { return f(v##N); }            \
    }

// clang-format off
_VN(0);  _VN(1);  _VN(2);  _VN(3);  _VN(4);  _VN(5);  _VN(6);  _VN(7);
_VN(8);  _VN(9); _VN(10); _VN(11); _VN(12); _VN(13); _VN(14); _VN(15);
// Do we need more than 16?
// clang-format on

template <usize I, typename... Ts>
struct _Tuple;

template <usize I>
struct _Tuple<I> {
    constexpr _Tuple() {}

    constexpr static usize len() { return 0; }

    constexpr static auto inspect(auto) {}

    constexpr auto visit(auto) {}
};

template <usize I, typename T>
struct _Tuple<I, T> :
    public _V<I, T> {

    constexpr _Tuple(T &&v)
        : _V<I, T>{std::forward<T>(v)} {}

    constexpr static usize len() {
        return 1;
    }

    constexpr static auto inspect(auto f) {
        return f(Meta::Type<T>{});
    }

    constexpr auto visit(auto f) {
        return _V<I, T>::visit(f);
    }
};

template <usize I, typename T, typename... Ts>
struct _Tuple<I, T, Ts...> :
    public _V<I, T>,
    public _Tuple<I + 1, Ts...> {

    constexpr _Tuple(T &&v, Ts &&...vs)
        : _V<I, T>{std::forward<T>(v)},
          _Tuple<I + 1, Ts...>{std::forward<Ts>(vs)...} {}

    static constexpr usize len() {
        return sizeof...(Ts) + 1;
    }

    constexpr static auto inspect(auto f) {
        f(Meta::Type<T>{});
        return _Tuple<I + 1, Ts...>::inspect(f);
    }

    constexpr auto visit(auto f) {
        _V<I, T>::visit(f);
        return _Tuple<I + 1, Ts...>::visit(f);
    }
};

template <typename... Ts>
struct Tuple : public _Tuple<0, Ts...> {
    using _Tuple<0, Ts...>::_Tuple;
};

template <typename... Ts>
Tuple(Ts &&...) -> Tuple<Ts...>;

} // namespace Karm
