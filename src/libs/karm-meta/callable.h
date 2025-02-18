#pragma once

#include <karm-base/base.h>

#include "cvrp.h"
#include "decl.h"

namespace Karm::Meta {

template <typename>
constexpr bool _Func = false;

template <typename Ret, typename... Args>
constexpr bool _Func<Ret(Args...)> = true;

template <typename Ret, typename... Args>
constexpr bool _Func<Ret(Args..., ...)> = true;

template <typename T>
concept Func = _Func<RemoveConstVolatileRef<T>>;

template <typename T>
concept FuncPtr = Ptr<T> and Meta::Func<RemovePtr<T>>;

template <typename U, typename... Args>
using Ret = decltype(Meta::declval<U>()(std::forward<Args>(Meta::declval<Args>())...));

template <typename T, typename... Args>
concept Callable = requires(T f) {
    f(declval<Args>()...);
};


} // namespace Karm::Meta
