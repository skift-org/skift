#include "value.h"

namespace Karm::Meta {

template <typename>
struct _Callable : False {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...)> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...)> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) const> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) const> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) volatile> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) volatile> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) const volatile> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) const volatile> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) &> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) &> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) const &> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) const &> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) volatile &> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) volatile &> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) const volatile &> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) const volatile &> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) &&> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) &&> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) const &&> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) const &&> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) volatile &&> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) volatile &&> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) const volatile &&> : True {
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) const volatile &&> : True {
};

template <typename T>
concept Callable = _Callable<T>::VALUE;

} // namespace Karm::Meta
