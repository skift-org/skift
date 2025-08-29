#pragma once

import Karm.Core;

#include <karm-core/macros.h>

#include "mem.h"

using namespace Karm;

namespace Hal {

struct Pmm;

enum struct PmmFlags : u64 {
    LOWER = 1 << 0,
    UPPER = 1 << 1,
    DMA = 1 << 2,
};

using PmmRange = Range<usize, struct PmmRangeTag>;
using PmmMem = Mem<Pmm, PmmRange>;

struct Pmm {
    using enum PmmFlags;

    virtual ~Pmm() = default;

    virtual Res<PmmRange> allocRange(usize size, Flags<PmmFlags> flags) = 0;

    Res<PmmMem> allocOwned(usize size, Flags<PmmFlags> flags) {
        return Ok(PmmMem{*this, try$(allocRange(size, flags))});
    }

    virtual Res<> used(PmmRange range, Flags<PmmFlags> flags = {}) = 0;

    virtual Res<> free(PmmRange range) = 0;
};

} // namespace Hal
