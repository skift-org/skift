#pragma once

#include <karm-meta/cond.h>

namespace Karm::Meta
{

template <typename T>
static constexpr bool _Const = false;

template <typename T>
static constexpr bool _Const<const T> = true;

template <typename T>
concept Const = _Const<T>;

template <typename T>
using AddConst = T const;

template <typename T>
struct _RemoveConst
{
    using Type = T;
};

template <typename T>
struct _RemoveConst<T const>
{
    using Type = T;
};

template <typename T>
using RemoveConst = typename _RemoveConst<T>::Type;

template <typename Src, typename Dest>
using CopyConst = Cond<Const<Src>, AddConst<Dest>, RemoveConst<Dest>>;

} // namespace Karm::Meta
