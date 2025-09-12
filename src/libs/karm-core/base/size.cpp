export module Karm.Core:base.size;

import :base.base;

namespace Karm {

export constexpr usize kib(usize v) { return v * 1024; }

export constexpr usize mib(usize v) { return kib(v * 1024); }

export constexpr usize gib(usize v) { return mib(v * 1024); }

export constexpr usize tib(usize v) { return gib(v * 1024); }

export constexpr usize toKib(usize v) { return v / 1024; }

export constexpr usize toMib(usize v) { return toKib(v) / 1024; }

export constexpr usize toGib(usize v) { return toMib(v) / 1024; }

export constexpr usize toTib(usize v) { return toGib(v) / 1024; }

} // namespace Karm
