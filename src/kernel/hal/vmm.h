#pragma once

#include <karm-base/enum.h>
#include <karm-base/range.h>
#include <karm-base/result.h>

#include "pmm.h"

namespace Hal {

struct Vmm;

enum struct VmmFlags : uint64_t {
    NIL = 0,
    READ = (1 << 0),
    WRITE = (1 << 1),
    EXEC = (1 << 2),
    USER = (1 << 3),
    GLOBAL = (1 << 4),
    UNCACHED = (1 << 5),
};

FlagsEnum$(VmmFlags);

using VmmRange = Range<size_t, struct VmmRangeTag>;

inline VmmRange identityMapped(PmmRange range) {
    return {range.start, range.size};
}

inline VmmRange upperHalfMapped(PmmRange range) {
    return {UPPER_HALF + range.start, range.size};
}

using VmmMem = Mem<Vmm, VmmRange>;

struct Vmm {
    using enum VmmFlags;

    virtual ~Vmm() = default;

    virtual Result<VmmRange> allocRange(VmmRange vaddr, PmmRange paddr, VmmFlags flags) = 0;

    Result<VmmMem> mapOwned(VmmRange vaddr, PmmRange paddr, VmmFlags flags) {
        return VmmMem{*this, try$(allocRange(vaddr, paddr, flags))};
    }

    virtual Error free(VmmRange vaddr) = 0;

    virtual Error update(VmmRange vaddr, VmmFlags flags) = 0;

    virtual Error flush(VmmRange vaddr) = 0;

    virtual void dump() = 0;

    virtual void activate() = 0;

    virtual size_t root() = 0;
};

} // namespace Hal
