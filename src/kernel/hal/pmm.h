#pragma once

#include <karm-base/enum.h>
#include <karm-base/range.h>
#include <karm-base/result.h>

#include "mem.h"

namespace Hal {

enum struct PmmFlags : uint64_t {
    NIL = 0,
    LOWER = (1 << 0),
    UPPER = (1 << 1),
    DMA = (1 << 2),
};

FlagsEnum$(PmmFlags);

using PmmRange = Range<size_t, struct PmmRangeTag>;

struct Pmm {
    using enum PmmFlags;
    using Flags = PmmFlags;
    using Range = PmmRange;

    virtual ~Pmm() = default;

    virtual Result<PmmRange> alloc(size_t size, PmmFlags flags) = 0;

    virtual Error used(PmmRange range, PmmFlags flags) = 0;

    virtual Error free(PmmRange range) = 0;
};

} // namespace Hal
