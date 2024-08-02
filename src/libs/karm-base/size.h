#pragma once

#include <karm-base/base.h>

namespace Karm {

inline constexpr usize kib(usize v) { return v * 1024; }

inline constexpr usize mib(usize v) { return kib(v * 1024); }

inline constexpr usize gib(usize v) { return mib(v * 1024); }

inline constexpr usize tib(usize v) { return gib(v * 1024); }

inline constexpr usize toKib(usize v) { return v / 1024; }

inline constexpr usize toMib(usize v) { return toKib(v) / 1024; }

inline constexpr usize toGib(usize v) { return toMib(v) / 1024; }

inline constexpr usize toTib(usize v) { return toGib(v) / 1024; }

} // namespace Karm
