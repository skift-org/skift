#pragma once

#include <karm-base/enum.h>
#include <karm-base/std.h>

namespace Karm::Sys {

enum MmapFlags : uint64_t {
    READ = (1 << 0),
    WRITE = (1 << 1),
    EXEC = (1 << 2),
    DMA = (1 << 3),
    STACK = (1 << 4),
    LOWER = (1 << 5),
    UPPER = (1 << 6),
};

FlagsEnum$(MmapFlags);

struct MmapOptions {
    MmapFlags flags;
    size_t vaddr;
    size_t paddr;
    size_t offset;
    size_t size;
};

} // namespace Karm::Sys
