#pragma once

#include "opt.h"
#include "std.h"

namespace Karm {

template <typename T>
struct Ref {
    T *_ptr{};

    Ref() = delete;
    Ref(T &ref) : _ptr(&ref) {}
    operator T &() { return *_ptr; }
};

template <typename T>
using OptRef = Opt<Ref<T>>;

} // namespace Karm
