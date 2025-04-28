#include <karm-base/bits.h>
#include <karm-base/lock.h>
#include <karm-base/size.h>
#include <karm-logger/logger.h>

#include "arch.h"
#include "mem.h"

namespace Hjert::Core {

struct Pmm : public Hal::Pmm {
    Hal::PmmRange _usable;
    Bits _bits;
    Lock _lock;

    Pmm(Hal::PmmRange usable, Bits bits)
        : _usable(usable),
          _bits(bits) {
        clear();
    }

    Res<Hal::PmmRange> allocRange(usize size, Hal::PmmFlags flags) override {
        LockScope scope(_lock);
        auto upper = (flags & Hal::PmmFlags::UPPER) == Hal::PmmFlags::UPPER;
        upper = false;

        try$(ensureAlign(size, Hal::PAGE_SIZE));
        size /= Hal::PAGE_SIZE;
        auto prange = bits2Pmm(try$(_bits.alloc(size, upper ? -1 : 0, upper)));
        return Ok(prange);
    }

    Res<> used(Hal::PmmRange prange, Hal::PmmFlags) override {
        if (not prange.overlaps(_usable))
            return Error::invalidInput("range is not in usable memory");

        LockScope scope(_lock);
        try$(prange.ensureAligned(Hal::PAGE_SIZE));
        _bits.set(pmm2Bits(prange), true);
        return Ok();
    }

    Res<> free(Hal::PmmRange prange) override {
        if (not prange.overlaps(_usable)) {
            return Error::invalidInput("range is not in usable memory");
        }

        LockScope scope(_lock);
        try$(prange.ensureAligned(Hal::PAGE_SIZE));
        _bits.set(pmm2Bits(prange), false);
        return Ok();
    }

    void clear() {
        LockScope scope(_lock);
        _bits.fill(true);
    }

    void dump() {
        logInfo(" mem: physical memory layout:");
        _bits.visit([this](auto range) {
            auto prange = bits2Pmm(range);
            logInfo("    {x} - {x} ({}kib)", prange.start, prange.end(), prange.size / kib(1));
        });
    }

    BitsRange pmm2Bits(Hal::PmmRange range) {
        range.start -= _usable.start;
        range.start /= Hal::PAGE_SIZE;
        range.size /= Hal::PAGE_SIZE;

        return range.into<BitsRange>();
    }

    Hal::PmmRange bits2Pmm(BitsRange range) {
        range.size *= Hal::PAGE_SIZE;
        range.start *= Hal::PAGE_SIZE;
        range.start += _usable.start;

        return range.into<Hal::PmmRange>();
    }
};

struct Kmm : public Hal::Kmm {
    Hal::Pmm& _pmm;

    Kmm(Hal::Pmm& pmm) : _pmm(pmm) {
    }

    Res<Hal::KmmRange> allocRange(usize size) override {
        auto range = try$(pmm2Kmm(try$(_pmm.allocRange(size, Hal::PmmFlags::NONE))));
        return Ok(range);
    }

    Res<> free(Hal::KmmRange range) override {
        return _pmm.free(try$(kmm2Pmm(range)));
    }

    Res<Hal::PmmRange> kmm2Pmm(Hal::KmmRange range) override {
        if (range.start < Hal::UPPER_HALF) {
            return Error::invalidInput("Invalid heap range");
        }

        return Ok(Hal::PmmRange{
            range.start - Hal::UPPER_HALF,
            range.size,
        });
    }

    Res<Hal::KmmRange> pmm2Kmm(Hal::PmmRange range) override {
        return Ok(Hal::KmmRange{
            range.start + Hal::UPPER_HALF,
            range.size,
        });
    }
};

static Opt<Pmm> _pmm = NONE;
static Opt<Kmm> _kmm = NONE;

Hal::PmmRange _findBitmapSpace(Handover::Payload& payload, usize bitmapSize) {
    for (auto& record : payload) {
        if (record.tag != Handover::Tag::FREE)
            continue;

        if (record.start == 0 and (record.size >= bitmapSize + Hal::PAGE_SIZE))
            return {static_cast<usize>(record.start) + Hal::PAGE_SIZE, bitmapSize};

        if (record.size >= bitmapSize)
            return {static_cast<usize>(record.start), bitmapSize};
    }

    logFatal("mem: no usable memory for bitmap");
}

Res<> initMem(Handover::Payload& payload) {
    auto usableRange = payload.usableRange<Hal::PmmRange>();

    if (usableRange.empty()) {
        logError("mem: no usable memory");
        return Error::outOfMemory("no usable memory");
    }

    logInfo("mem: usable range: {x}-{x}", usableRange.start, usableRange.end());

    usize bitsSize = Hal::pageAlignUp(usableRange.size / Hal::PAGE_SIZE / 8);

    auto pmmBits = _findBitmapSpace(payload, bitsSize);

    if (pmmBits.empty()) {
        logError("mem: no usable memory for pmm");
        return Error::outOfMemory("no usable memory for pmm");
    }

    logInfo("mem: pmm bitmap range: {p}-{p}", pmmBits.start, pmmBits.end());

    _pmm.emplace(
        usableRange,
        MutSlice{
            reinterpret_cast<u8*>(pmmBits.start + Hal::UPPER_HALF),
            pmmBits.size,
        }
    );

    _kmm.emplace(_pmm.unwrap());

    logInfo("mem: marking free memory as free...");
    for (auto& record : payload) {
        if (record.tag == Handover::Tag::FREE) {
            logInfo("mem: free memory at {p} {p} ({}kib)", record.start, record.start + record.size, record.size / kib(1));
            try$(pmm().free({static_cast<usize>(record.start), static_cast<usize>(record.size)}));
        }
    }

    Hal::PmmRange firstPage = {0, Hal::PAGE_SIZE};

    if (firstPage.overlaps(usableRange)) {
        logInfo("mem: marking first page as used...");
        try$(pmm().used(firstPage, Hal::PmmFlags::NONE));
    }

    try$(pmm().used({pmmBits.start, pmmBits.size}, Hal::PmmFlags::NONE));

    _pmm->dump();

    logInfo("mem: mapping kernel...");
    try$(Arch::globalVmm().mapRange({
                                        Handover::KERNEL_BASE + Hal::PAGE_SIZE,
                                        gib(2) - Hal::PAGE_SIZE - Hal::PAGE_SIZE,
                                    },
                                    {
                                        Hal::PAGE_SIZE,
                                        gib(2) - Hal::PAGE_SIZE - Hal::PAGE_SIZE,
                                    },
                                    Hal::Vmm::READ | Hal::Vmm::WRITE));

    logInfo("mem: mapping upper half...");
    try$(Arch::globalVmm().mapRange({
                                        Handover::UPPER_HALF + Hal::PAGE_SIZE,
                                        gib(4) - Hal::PAGE_SIZE,
                                    },
                                    {
                                        Hal::PAGE_SIZE,
                                        gib(4) - Hal::PAGE_SIZE,
                                    },
                                    Hal::Vmm::READ | Hal::Vmm::WRITE));

    Arch::globalVmm().activate();

    return Ok();
}

Hal::Pmm& pmm() {
    if (not _pmm)
        logFatal("mem: pmm not initialized yet");
    return *_pmm;
}

Hal::Kmm& kmm() {
    if (not _kmm)
        logFatal("mem: heap not initialized yet");
    return *_kmm;
}

} // namespace Hjert::Core
