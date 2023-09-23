#pragma once

#include "range.h"
#include "res.h"
#include "vec.h"

namespace Karm {

template <typename R = USizeRange>
struct RangeAlloc {
    Vec<R> _r;

    void used(R range) {
        for (usize i = 0; i < _r.len(); i++) {
            R curr = _r[i];

            if (curr == range) {
                _r.removeAt(i);
                break;
            }

            if (curr.overlaps(range)) {
                R lh = curr.halfUnder(range);
                R uh = curr.halfOver(range);

                if (lh.size != 0) {
                    _r[i] = lh;

                    if (uh.size != 0) {
                        unused(uh);
                    }
                } else if (uh.size != 0) {
                    _r[i] = uh;
                }
            }
        }
    }

    Res<R> alloc(usize size) {
        for (usize i = 0; i < _r.len(); i++) {
            if (_r[i].size == size) {
                R result = _r[i];
                _r.removeAt(i);
                return Ok(result);
            }

            if (_r[i].size > size) {
                R result = {_r[i].start, size};
                _r[i].start += size;
                _r[i].size -= size;
                return Ok(result);
            }
        }

        return Error::outOfMemory();
    }

    void compress(usize start) {
        while (start + 1 < _r.len() and _r[start].contigous(_r[start + 1])) {
            _r[start] = _r[start].merge(_r[start + 1]);
            _r.removeAt(start + 1);
        }

        while (start > 0 and _r[start].contigous(_r[start - 1])) {
            _r[start] = _r[start].merge(_r[start - 1]);
            _r.removeAt(start - 1);
        }
    }

    void unused(R range) {
        for (usize i = 0; i < _r.len(); i++) {
            if (_r[i].contigous(range)) {
                _r[i] = _r[i].merge(range);
                compress(i);
                return;
            }

            if (i + 1 < _r.len() &&
                (_r[i].start < range.start) &&
                (range.start < _r[i + 1].start)) {
                _r.insert(i + 1, range);
                return;
            }
        }

        _r.pushBack(range);
    }
};

} // namespace Karm
