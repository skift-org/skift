#pragma once

#include "value.h"

namespace Karm::Meta
{
template <typename T>
concept Trivial = __is_trivial(T);

template <typename Derived, typename Base>
concept Derive = __is_base_of(Base, Derived);

template <typename>
struct _Callable : False
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...)> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...)> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) const> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) const> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) volatile> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) volatile> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) const volatile> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) const volatile> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) &> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) &> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) const &> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) const &> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) volatile &> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) volatile &> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) const volatile &> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) const volatile &> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) &&> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) &&> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) const &&> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) const &&> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) volatile &&> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) volatile &&> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args...) const volatile &&> : True
{
};

template <typename Ret, typename... Args>
struct _Callable<Ret(Args..., ...) const volatile &&> : True
{
};

template <typename T>
concept Callable = _Callable<T>::VALUE;


} // namespace Karm::Meta
