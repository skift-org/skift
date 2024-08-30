#pragma once

#include <karm-base/enum.h>
#include <karm-base/range.h>
#include <karm-base/res.h>

#include "pmm.h"

namespace Hal {

struct Vmm;

enum struct VmmFlags : u64 {
    NONE = 0,
    READ = (1 << 0),
    WRITE = (1 << 1),
    EXEC = (1 << 2),
    USER = (1 << 3),
    GLOBAL = (1 << 4),
    UNCACHED = (1 << 5),
};

FlagsEnum$(VmmFlags);

using VmmRange = Range<usize, struct _VmmRangeTag>;

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

    virtual Res<VmmRange> mapRange(VmmRange vaddr, PmmRange paddr, VmmFlags flags) = 0;

    Res<VmmMem> mapOwned(VmmRange vaddr, PmmRange paddr, VmmFlags flags) {
        auto range = try$(mapRange(vaddr, paddr, flags));
        return Ok(VmmMem{*this, range});
    }

    virtual Res<> free(VmmRange vaddr) = 0;

    virtual Res<> update(VmmRange vaddr, VmmFlags flags) = 0;

    virtual Res<> flush(VmmRange vaddr) = 0;

    virtual void dump() = 0;

    virtual void activate() = 0;

    virtual usize root() = 0;
};

} // namespace Hal
