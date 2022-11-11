#pragma once

#include <karm-base/range.h>

#include "pmm.h"

namespace Hal {

struct HeapRange : public USizeRange {
    using USizeRange::USizeRange;
};

struct Heap {
    virtual ~Heap() = default;

    virtual Result<HeapRange> alloc(size_t size) = 0;

    virtual Error free(HeapRange range) = 0;

    virtual Result<PmmRange> heap2Pmm(HeapRange range) = 0;

    virtual Result<HeapRange> pmm2Heap(PmmRange range) = 0;
};

} // namespace Hal
