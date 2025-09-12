#pragma once

import Karm.Core;

namespace Karm::Sys {

using Handle = Distinct<usize, struct _HandleTag>;

static constexpr Handle INVALID = Handle(-1);

enum struct MmapOption : u64 {
    READ = 1 << 0,
    WRITE = 1 << 1,
    EXEC = 1 << 2,
    DMA = 1 << 3,
    STACK = 1 << 4,
    LOWER = 1 << 5,
    UPPER = 1 << 6,
    PREFETCH = 1 << 7,
};

struct MmapProps {
    Flags<MmapOption> options = {};
    usize vaddr = 0;
    usize paddr = 0;
    usize offset = 0;
    usize size = 0;
};

struct MmapResult {
    usize paddr;
    usize vaddr;
    usize size;
};

} // namespace Karm::Sys
