#pragma once

namespace Vaev {

template <typename T>
struct ValueContext;

template <typename T>
struct ValueContext {
    using Resolved = T;

    Resolved resolve(T value) {
        return value;
    }
};

} // namespace Vaev
