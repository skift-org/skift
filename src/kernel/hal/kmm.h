#pragma once

#include <karm-base/range.h>
#include <karm-base/slice.h>
#include <karm-meta/nocopy.h>

#include "pmm.h"

namespace Hal {

struct Kmm;

struct KmmRange : public Range<usize, struct KmmRangeTag> {
    using Range::Range;

    Bytes bytes() const {
        return Bytes{(Byte const*)start, size};
    }

    MutBytes mutBytes() {
        return MutBytes{(Byte*)start, size};
    }

    template <typename T>
    T* as() {
        return (T*)start;
    }

    template <typename T>
    T const* as() const {
        return (T const*)start;
    }
};

using KmmMem = Mem<Kmm, KmmRange>;

struct Kmm {
    virtual ~Kmm() = default;

    virtual Res<KmmRange> allocRange(usize size) = 0;

    Res<KmmMem> allocOwned(usize size) {
        return Ok(KmmMem{*this, try$(allocRange(size))});
    }

    virtual Res<> free(KmmRange range) = 0;

    virtual Res<PmmRange> kmm2Pmm(KmmRange range) = 0;

    virtual Res<KmmRange> pmm2Kmm(PmmRange range) = 0;
};

} // namespace Hal
