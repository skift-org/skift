#pragma once

namespace Karm::Meta {

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
struct _RemovePtr<T *const> {
    using Type = T;
};

template <typename T>
struct _RemovePtr<T *volatile> {
    using Type = T;
};

template <typename T>
struct _RemovePtr<T *const volatile> {
    using Type = T;
};

template <typename T>
using RemovePtr = typename _RemovePtr<T>::Type;

} // namespace Karm::Meta
