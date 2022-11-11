#pragma once

#include <karm-base/enum.h>
#include <karm-base/range.h>
#include <karm-base/result.h>

#include "pmm.h"

namespace Hal {

enum struct VmmFlags : uint64_t {
    NIL = 0,
    READ = (1 << 0),
    WRITE = (1 << 1),
    EXEC = (1 << 2),
    USER = (1 << 3),
    GLOBAL = (1 << 4),
    WRITETHROUGH = (1 << 5),
};

FlagsEnum$(VmmFlags);

using VmmRange = Range<size_t, struct VmmRangeTag>;

inline VmmRange identityMapped(PmmRange range) {
    return {range.start, range.size};
}

inline VmmRange upperHalfMapped(PmmRange range) {
    return {UPPER_HALF + range.start, range.size};
}

struct Vmm {
    using enum VmmFlags;
    using Flags = VmmFlags;
    using Range = VmmRange;

    virtual ~Vmm() = default;

    virtual Error map(VmmRange vaddr, PmmRange paddr, VmmFlags flags) = 0;

    virtual Error unmap(VmmRange vaddr) = 0;

    virtual Error update(VmmRange vaddr, VmmFlags flags) = 0;

    virtual Error flush(VmmRange vaddr) = 0;

    virtual void dump() = 0;

    virtual void activate() = 0;

    virtual size_t root() = 0;
};

} // namespace Hal
