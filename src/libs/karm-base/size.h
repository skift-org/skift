#pragma once

#include "std.h"

namespace Karm {

static inline size_t kib(size_t v) { return v * 1024; }
static inline size_t mib(size_t v) { return kib(v * 1024); }
static inline size_t gib(size_t v) { return mib(v * 1024); }
static inline size_t tib(size_t v) { return gib(v * 1024); }

} // namespace Karm
