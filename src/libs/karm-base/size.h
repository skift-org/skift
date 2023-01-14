#pragma once

#include "std.h"

namespace Karm {

inline constexpr size_t kib(size_t v) { return v * 1024; }
inline constexpr size_t mib(size_t v) { return kib(v * 1024); }
inline constexpr size_t gib(size_t v) { return mib(v * 1024); }
inline constexpr size_t tib(size_t v) { return gib(v * 1024); }

inline constexpr size_t toKib(size_t v) { return v / 1024; }
inline constexpr size_t toMib(size_t v) { return toKib(v) / 1024; }
inline constexpr size_t toGib(size_t v) { return toMib(v) / 1024; }
inline constexpr size_t toTib(size_t v) { return toGib(v) / 1024; }

} // namespace Karm
