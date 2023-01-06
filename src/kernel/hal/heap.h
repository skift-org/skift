#pragma once

#include <karm-base/range.h>
#include <karm-meta/nocopy.h>

#include "pmm.h"

namespace Hal {

struct Heap;

using HeapRange = Range<size_t, struct HeapRangeTag>;

using HeapMem = Mem<Heap, HeapRange>;

struct Heap {
    virtual ~Heap() = default;

    virtual Result<HeapRange> allocRange(size_t size) = 0;

    Result<HeapMem> allocOwned(size_t size) {
        return HeapMem{*this, try$(allocRange(size))};
    }

    virtual Error free(HeapRange range) = 0;

    virtual Result<PmmRange> heap2Pmm(HeapRange range) = 0;

    virtual Result<HeapRange> pmm2Heap(PmmRange range) = 0;
};

} // namespace Hal
