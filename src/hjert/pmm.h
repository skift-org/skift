#pragma once

#include <hal/mem.h>
#include <hal/pmm.h>
#include <karm-base/bits.h>

namespace Hjert {
struct HjertPmm : public Hal::Pmm {
    Karm::Bits _bitmap;
    size_t _bestbet_lower = 0;
    size_t _bestbet_upper = (size_t)-1;
    Hal::PmmRange _usable_range;

    Karm::BitsRange _pages_range;

    Karm::BitsRange _addr2page(Hal::PmmRange range) const {
        range._start -= _usable_range._start;
        return range / Hal::PAGE_SIZE;
    }

    Hal::PmmRange _page2addr(Karm::BitsRange range) const {
        Hal::PmmRange result = (range * Hal::PAGE_SIZE).as<Hal::PmmRange>();
        result._start += _usable_range._start;
        return result;
    }

    Result<Hal::PmmRange> alloc(size_t size, Hal::PmmFlags flags) override {
        if (Hal::isPageAlign(size)) {
            return Error::INVALID_INPUT;
        }
        auto page_count = size / Hal::PAGE_SIZE;
        bool upper = (flags & Hal::PmmFlags::UPPER) == Hal::PmmFlags::UPPER;

        auto range = _bitmap.alloc(page_count, upper ? _bestbet_upper : _bestbet_lower, upper);

        if (!range->any()) {
            if (upper) {
                _bestbet_upper = (size_t)-1;
            } else {
                _bestbet_lower = 0;
            }
            range = _bitmap.alloc(page_count, upper ? _bestbet_upper : _bestbet_lower);
        }

        if (upper) {
            _bestbet_upper = range->start();
        } else {
            _bestbet_lower = range->end();
        }

        if (!range->any()) {

            return Error::OUT_OF_MEMORY;
        }

        return _page2addr(range.take());
    }

    Error used(Hal::PmmRange range, Hal::PmmFlags) override {

        auto bit_range = _addr2page(range);

        _bitmap.set(bit_range, true);

        return OK;
    }

    Error free(Hal::PmmRange range) override {

        auto bit_range = _addr2page(range);

        _bitmap.set(bit_range, false);

        return OK;
    }
};
} // namespace Hjert