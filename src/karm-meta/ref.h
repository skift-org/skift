#pragma once

#include "value.h"

namespace Karm::Meta {

template <class T>
struct _RemoveRef {
    using Type = T;
};

template <class T>
struct _RemoveRef<T &> {
    using Type = T;
};

template <class T>
struct _RemoveRef<T &&> {
    using Type = T;
};

template <typename T>
using RemoveRef = typename _RemoveRef<T>::Type;

template <typename T>
struct _LvalueRef : False {
};

template <typename T>
struct _LvalueRef<T &> : True {
};

template <typename T>
concept LvalueRef = _LvalueRef<T>::VALUE;

template <typename T>
struct _RvalueRef : False {
};

template <typename T>
struct _RvalueRef<T &&> : True {
};

template <typename T>
concept RvalueRef = _RvalueRef<T>::VALUE;



} // namespace Karm::Meta
