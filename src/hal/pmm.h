#pragma once

#include <karm-base/enum.h>
#include <karm-base/range.h>
#include <karm-base/result.h>

namespace Hal {

enum PmmFlags : uint64_t {
    LOWER = (1 << 0),
    UPPER = (1 << 1),
    DMW = (1 << 2),
};

FlagsEnum$(PmmFlags);

struct PmmRange : public USizeRange {
    using USizeRange::USizeRange;
};

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
