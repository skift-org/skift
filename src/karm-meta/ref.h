#pragma once

#include "value.h"

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

template <typename T>
struct _LvalueRef : False
{
};

template <typename T>
struct _LvalueRef<T &> : True
{
};

template <typename T>
concept LvalueRef = _LvalueRef<T>::VALUE;

template <typename T>
struct _RvalueRef : False
{
};

template <typename T>
struct _RvalueRef<T &&> : True
{
};

template <typename T>
concept RvalueRef = _RvalueRef<T>::VALUE;

template <typename T>
struct _Ptr : False
{
};

template <typename T>
struct _Ptr<T *> : True
{
};

template <typename T>
concept Ptr = _Ptr<T>::VALUE;

template <typename T>
struct _RemovePtr
{
    using Type = T;
};

template <typename T>
struct _RemovePtr<T *>
{
    using Type = T;
};

template <typename T>
struct _RemovePtr<T *const>
{
    using Type = T;
};

template <typename T>
struct _RemovePtr<T *volatile>
{
    using Type = T;
};

template <typename T>
struct _RemovePtr<T *const volatile>
{
    using Type = T;
};

template <typename T>
using RemovePtr = typename _RemovePtr<T>::Type;

} // namespace Karm::Meta
