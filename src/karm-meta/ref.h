#pragma once

namespace Karm::Meta
{

template <class T>
struct _RemoveRef
{
    using Type = T;
};

template <class T>
struct _RemoveRef<T &>
{
    using Type = T;
};

template <class T>
struct _RemoveRef<T &&>
{
    using Type = T;
};

template <typename T>
using RemoveRef = typename _RemoveRef<T>::Type;

} // namespace Karm::Meta
