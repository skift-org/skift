#pragma once

#include <karm-meta/id.h>

#include "_prelude.h"

#include "opt.h"

namespace Karm::Base {

template <typename T>
struct Ref {
    T *_ptr;

    Ref() = delete;
    Ref(T &ref) : _ptr(&ref) {}
    operator T &() { return *_ptr; }
};

template <typename T>
using OptRef = Opt<Ref<T>>;

} // namespace Karm::Base
