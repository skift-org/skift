#pragma once

#include <karm-base/distinct.h>
#include <karm-base/enum.h>

#include "_handle.h"

namespace Karm::Sys {

enum MmapFlags : u64 {
    NIL = 0,
    READ = (1 << 0),
    WRITE = (1 << 1),
    EXEC = (1 << 2),
    DMA = (1 << 3),
    STACK = (1 << 4),
    LOWER = (1 << 5),
    UPPER = (1 << 6),
    PREFETCH = (1 << 7),
};

FlagsEnum$(MmapFlags);

struct MmapOptions {
    MmapFlags flags = MmapFlags::NIL;
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
