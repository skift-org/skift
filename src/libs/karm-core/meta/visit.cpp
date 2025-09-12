export module Karm.Core:meta.visit;

import :base.std;
import :base.base;
import :meta.cvrp;
import :meta.traits;

namespace Karm::Meta {

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

export template <class T, usize Bases = 0u, class... Ts>
    requires Meta::Aggregate<T>
constexpr auto len() -> usize {
    if constexpr (requires { T{Ts{}...}; } and not requires { T{Ts{}..., _VisitAny{}}; }) {
        return sizeof...(Ts) - Bases;
    } else if constexpr (Bases == sizeof...(Ts) and requires { T{Ts{}...}; } and not requires { T{Ts{}..., _VisitExcept<T>{}}; }) {
        return len<T, Bases + 1u, Ts..., _VisitAny>();
    } else {
        return len<T, Bases, Ts..., _VisitAny>();
    }
}

template <typename T>
auto _visit(T&, auto f, std::integral_constant<usize, 0>) {
    return f();
}

template <typename T>
auto _visit(T& v, auto f, std::integral_constant<usize, 1>) {
    auto& [_last] = v;
    return f(_last);
}

template <typename T>
auto _visit(T& v, auto f, std::integral_constant<usize, 2>) {
    auto& [_0, _last] = v;
    if (auto res = f(_0); not res)
        return res;
    return f(_last);
}

template <typename T>
auto _visit(T& v, auto f, std::integral_constant<usize, 3>) {
    auto& [_0, _1, _last] = v;
    if (auto res = f(_0); not res)
        return res;
    if (auto res = f(_1); not res)
        return res;
    return f(_last);
}

template <typename T>
auto _visit(T& v, auto f, std::integral_constant<usize, 4>) {
    auto& [_0, _1, _2, _last] = v;
    if (auto res = f(_0); not res)
        return res;
    if (auto res = f(_1); not res)
        return res;
    if (auto res = f(_2); not res)
        return res;
    return f(_last);
}

template <typename T>
auto _visit(T& v, auto f, std::integral_constant<usize, 5>) {
    auto& [_0, _1, _2, _3, _last] = v;
    if (auto res = f(_0); not res)
        return res;
    if (auto res = f(_1); not res)
        return res;
    if (auto res = f(_2); not res)
        return res;
    if (auto res = f(_3); not res)
        return res;
    return f(_last);
}

template <typename T>
auto _visit(T& v, auto f, std::integral_constant<usize, 6>) {
    auto& [_0, _1, _2, _3, _4, _last] = v;
    if (auto res = f(_0); not res)
        return res;
    if (auto res = f(_1); not res)
        return res;
    if (auto res = f(_2); not res)
        return res;
    if (auto res = f(_3); not res)
        return res;
    if (auto res = f(_4); not res)
        return res;
    return f(_last);
}

template <typename T>
auto _visit(T& v, auto f, std::integral_constant<usize, 7>) {
    auto& [_0, _1, _2, _3, _4, _5, _last] = v;
    if (auto res = f(_0); not res)
        return res;
    if (auto res = f(_1); not res)
        return res;
    if (auto res = f(_2); not res)
        return res;
    if (auto res = f(_3); not res)
        return res;
    if (auto res = f(_4); not res)
        return res;
    if (auto res = f(_5); not res)
        return res;
    return f(_last);
}

template <typename T>
auto _visit(T& v, auto f, std::integral_constant<usize, 8>) {
    auto& [_0, _1, _2, _3, _4, _5, _6, _last] = v;
    if (auto res = f(_0); not res)
        return res;
    if (auto res = f(_1); not res)
        return res;
    if (auto res = f(_2); not res)
        return res;
    if (auto res = f(_3); not res)
        return res;
    if (auto res = f(_4); not res)
        return res;
    if (auto res = f(_5); not res)
        return res;
    if (auto res = f(_6); not res)
        return res;
    return f(_last);
}

export template <typename T>
auto visit(T& t, auto f) {
    return _visit(t, f, len<T>());
}

} // namespace Karm::Meta