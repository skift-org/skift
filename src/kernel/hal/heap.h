#pragma once

#include <karm-base/range.h>
#include <karm-base/slice.h>
#include <karm-meta/nocopy.h>

#include "pmm.h"

namespace Hal {

struct Heap;

struct HeapRange : public Range<size_t, struct HeapRangeTag> {
    using Range::Range;

    Bytes bytes() const {
        return Bytes{(Byte const *)start, size};
    }

    MutBytes mutBytes() {
        return MutBytes{(Byte *)start, size};
    }
};

using HeapMem = Mem<Heap, HeapRange>;

struct Heap {
    virtual ~Heap() = default;

    virtual Res<HeapRange> allocRange(size_t size) = 0;

    Res<HeapMem> allocOwned(size_t size) {
        return Ok(HeapMem{*this, try$(allocRange(size))});
    }

    virtual Res<> free(HeapRange range) = 0;

    virtual Res<PmmRange> heap2Pmm(HeapRange range) = 0;

    virtual Res<HeapRange> pmm2Heap(PmmRange range) = 0;
};

} // namespace Hal
