export module Vaev.Engine:values.resolved;

namespace Vaev {

export template <typename T>
struct _Resolved {
    using Type = T;
};

export template <typename T>
using Resolved = typename _Resolved<T>::Type;

} // namespace Vaev
