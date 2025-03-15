#pragma once

namespace Vaev {

template <typename T>
struct _Resolved {
    using Type = T;
};

template <typename T>
using Resolved = _Resolved<T>::Type;

} // namespace Vaev
