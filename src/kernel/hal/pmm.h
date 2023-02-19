#pragma once

#include <karm-base/enum.h>
#include <karm-base/range.h>
#include <karm-base/res.h>
#include <karm-meta/nocopy.h>

#include "mem.h"

namespace Hal {

struct Pmm;

enum struct PmmFlags : uint64_t {
    NONE = 0,
    LOWER = (1 << 0),
    UPPER = (1 << 1),
    DMA = (1 << 2),
};

FlagsEnum$(PmmFlags);

using PmmRange = Range<size_t, struct PmmRangeTag>;
using PmmMem = Mem<Pmm, PmmRange>;

struct Pmm {
    using enum PmmFlags;

    virtual ~Pmm() = default;

    virtual Res<PmmRange> allocRange(size_t size, PmmFlags flags) = 0;

    Res<PmmMem> allocOwned(size_t size, PmmFlags flags) {
        return Ok(PmmMem{*this, try$(allocRange(size, flags))});
    }

    virtual Res<> used(PmmRange range, PmmFlags flags) = 0;

    virtual Res<> free(PmmRange range) = 0;
};

} // namespace Hal
