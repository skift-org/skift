#pragma once

#include <karm-base/bits.h>

#include "mem.h"
#include "pmm.h"

namespace Hal {

struct PmmAlloc : public Hal::Pmm {
    size_t _base{};
    Bits _bits{};

    size_t _bestbetLower{};
    size_t _bestbetUpper{(size_t)-1};

    PmmAlloc() = default;

    PmmAlloc(size_t base, Bits bits) : _base(base), _bits(bits) {}

    BitsRange _addr2page(Hal::PmmRange range) const {
        return {
            range.start() / Hal::PAGE_SIZE - _base,
            range.size() / Hal::PAGE_SIZE,
        };
    }

    Hal::PmmRange _page2addr(BitsRange range) const {
        return {
            range.start() * Hal::PAGE_SIZE + _base,
            range.size() * Hal::PAGE_SIZE,
        };
    }

    Result<Hal::PmmRange> alloc(size_t size, Hal::PmmFlags flags) override {
        if (!Hal::isPageAlign(size)) {
            return Error{Error::INVALID_INPUT, "size not page aligned"};
        }

        auto pageCount = size / Hal::PAGE_SIZE;
        bool upper = (flags & Hal::PmmFlags::UPPER) == Hal::PmmFlags::UPPER;

        auto maybeRange = _bits.alloc(pageCount, upper ? _bestbetUpper : _bestbetLower, upper);

        if (!maybeRange) {
            if (upper) {
                _bestbetUpper = (size_t)-1;
            } else {
                _bestbetLower = 0;
            }

            maybeRange = _bits.alloc(pageCount, upper ? _bestbetUpper : _bestbetLower);
        }

        if (upper) {
            _bestbetUpper = maybeRange->start();
        } else {
            _bestbetLower = maybeRange->end();
        }

        if (!maybeRange) {

            return Error::OUT_OF_MEMORY;
        }

        return _page2addr(maybeRange.take());
    }

    Error used(Hal::PmmRange range, Hal::PmmFlags) override {
        _bits.set(_addr2page(range), true);
        return OK;
    }

    Error free(Hal::PmmRange range) override {
        _bits.set(_addr2page(range), false);
        return OK;
    }
};

} // namespace Hal
