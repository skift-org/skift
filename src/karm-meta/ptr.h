#include "value.h"

namespace Karm::Meta {

template <typename T>
struct _Ptr : False {
};

template <typename T>
struct _Ptr<T *> : True {
};

template <typename T>
concept Ptr = _Ptr<T>::VALUE;

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
