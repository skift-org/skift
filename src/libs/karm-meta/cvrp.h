#pragma once

#include "cond.h"

namespace Karm::Meta {

// MARK: Const -----------------------------------------------------------------

template <typename T>
inline constexpr bool _Const = false;

template <typename T>
inline constexpr bool _Const<T const> = true;

template <typename T>
concept Const = _Const<T>;

template <typename T>
struct _RemoveConst {
    using Type = T;
};

template <typename T>
struct _RemoveConst<T const> {
    using Type = T;
};

template <typename T>
using RemoveConst = typename _RemoveConst<T>::Type;

template <typename Src, typename Dest>
using CopyConst = Cond<Const<Src>, Dest const, RemoveConst<Dest>>;

// MARK: Volatile --------------------------------------------------------------

template <typename T>
inline constexpr bool _Volatile = false;

template <typename T>
inline constexpr bool _Volatile<T volatile> = true;

template <typename T>
concept Volatile = _Volatile<T>;

template <typename T>
struct _RemoveVolatile {
    using Type = T;
};

template <typename T>
struct _RemoveVolatile<T volatile> {
    using Type = T;
};

template <typename T>
using RemoveVolatile = typename _RemoveVolatile<T>::Type;

template <typename Src, typename Dest>
using CopyVolatile = Cond<Volatile<Src>, Dest volatile, RemoveVolatile<Dest>>;

template <typename T>
using RemoveConstVolatile = RemoveConst<RemoveVolatile<T>>;

// MARK: Ref -------------------------------------------------------------------*/

template <typename>
inline constexpr bool _LvalueRef = false;

template <typename T>
inline constexpr bool _LvalueRef<T &> = true;

template <typename T>
concept LvalueRef = _LvalueRef<T>;

template <typename>
inline constexpr bool _RvalueRef = false;

template <typename T>
inline constexpr bool _RvalueRef<T &&> = true;

template <typename T>
concept RvalueRef = _RvalueRef<T>;

template <typename T>
concept Ref = LvalueRef<T> or RvalueRef<T>;

template <typename T>
struct _RemoveRef {
    using Type = T;
};

template <typename T>
struct _RemoveRef<T &> {
    using Type = T;
};

template <typename T>
struct _RemoveRef<T &&> {
    using Type = T;
};

template <typename T>
using RemoveRef = typename _RemoveRef<T>::Type;

template <typename T>
using RemoveConstVolatileRef = RemoveConstVolatile<RemoveRef<T>>;

// MARK: Pointer ---------------------------------------------------------------

template <typename>
inline constexpr bool _Ptr = false;

template <typename T>
inline constexpr bool _Ptr<T *> = true;

template <typename T>
concept Ptr = _Ptr<T>;

template <typename T>
struct _RemovePtr {
    using Type = T;
};

template <typename T>
struct _RemovePtr<T *> {
    using Type = T;
};

template <typename T>
using RemovePtr = typename _RemovePtr<RemoveConstVolatile<T>>::Type;

} // namespace Karm::Meta
