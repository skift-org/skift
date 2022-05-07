#include "ptr.h"
#include "utils.h"

namespace Karm::Meta {

template <typename>
inline constexpr bool _Func = false;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args...)> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args..., ...)> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args...) const> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args..., ...) const> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args...) volatile> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args..., ...) volatile> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args...) const volatile> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args..., ...) const volatile> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args...) &> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args..., ...) &> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args...) const &> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args..., ...) const &> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args...) volatile &> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args..., ...) volatile &> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args...) const volatile &> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args..., ...) const volatile &> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args...) &&> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args..., ...) &&> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args...) const &&> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args..., ...) const &&> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args...) volatile &&> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args..., ...) volatile &&> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args...) const volatile &&> = true;

template <typename Ret, typename... Args>
inline constexpr bool _Func<Ret(Args..., ...) const volatile &&> = true;

template <typename T>
concept Func = _Func<T>;

template <typename T>
concept FuncPtr = Ptr<T> and Meta::Func<RemovePtr<T>>;

template <typename T, typename... Args>
inline constexpr bool Callable = requires(T f) { t(declval<Args>()...); };

} // namespace Karm::Meta
