#pragma once

#include "cvrp.h"
#include "utils.h"

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

template <typename T, typename... Args>
constexpr bool Callable = requires(T f) { t(declval<Args>()...); };

} // namespace Karm::Meta
