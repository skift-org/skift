#pragma once

#include <karm-base/enum.h>
#include <karm-base/range.h>
#include <karm-base/res.h>
#include <karm-meta/nocopy.h>

#include "mem.h"

namespace Hal {

struct Pmm;

enum struct PmmFlags : u64 {
    NONE = 0,
    LOWER = (1 << 0),
    UPPER = (1 << 1),
    DMA = (1 << 2),
};

FlagsEnum$(PmmFlags);

using PmmRange = Range<usize, struct PmmRangeTag>;
using PmmMem = Mem<Pmm, PmmRange>;

struct Pmm {
    using enum PmmFlags;

    virtual ~Pmm() = default;

    virtual Res<PmmRange> allocRange(usize size, PmmFlags flags) = 0;

    Res<PmmMem> allocOwned(usize size, PmmFlags flags) {
        return Ok(PmmMem{*this, try$(allocRange(size, flags))});
    }

    virtual Res<> used(PmmRange range, PmmFlags flags) = 0;

    virtual Res<> free(PmmRange range) = 0;
};

} // namespace Hal
