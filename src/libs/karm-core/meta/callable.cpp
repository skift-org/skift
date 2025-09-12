export module Karm.Core:meta.callable;

import :base.base;
import :meta.cvrp;
import :meta.decl;

namespace Karm::Meta {

template <typename>
constexpr bool _Func = false;

template <typename Ret, typename... Args>
constexpr bool _Func<Ret(Args...)> = true;

template <typename Ret, typename... Args>
constexpr bool _Func<Ret(Args..., ...)> = true;

export template <typename T>
concept Func = _Func<RemoveConstVolatileRef<T>>;

export template <typename T>
concept FuncPtr = Ptr<T> and Meta::Func<RemovePtr<T>>;

export template <typename U, typename... Args>
using Ret = decltype(Meta::declval<U>()(std::forward<Args>(Meta::declval<Args>())...));

export template <typename T, typename... Args>
concept Callable = requires(T f) {
    f(declval<Args>()...);
};

} // namespace Karm::Meta
