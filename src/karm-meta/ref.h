#pragma once

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

} // namespace Karm::Meta
