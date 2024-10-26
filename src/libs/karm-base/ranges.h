#pragma once

#include "range.h"
#include "res.h"
#include "vec.h"

namespace Karm {

template <typename R = urange>
struct Ranges {
    Vec<R> _r;

    Slice<R> ranges() {
        return _r;
    }

    void clear() {
        _r.clear();
    }

    void remove(R range) {
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
                        add(uh);
                    }
                } else if (uh.size != 0) {
                    _r[i] = uh;
                }
            }
        }
    }

    Res<R> take(usize size) {
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

    void _compress(usize i) {
        while (i + 1 < _r.len() and _r[i].contigous(_r[i + 1])) {
            _r[i] = _r[i].merge(_r[i + 1]);
            _r.removeAt(i + 1);
        }

        while (i > 0 and _r[i].contigous(_r[i - 1])) {
            _r[i] = _r[i].merge(_r[i - 1]);
            _r.removeAt(i - 1);
        }
    }

    void add(R range) {
        for (usize i = 0; i < _r.len(); i++) {
            if (_r[i].contigous(range)) {
                _r[i] = _r[i].merge(range);
                _compress(i);
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

    bool contains(R range) const {
        for (usize i = 0; i < _r.len(); i++) {
            if (_r[i].contains(range)) {
                return true;
            }
        }

        return false;
    }

    bool colides(R range) const {
        for (usize i = 0; i < _r.len(); i++) {
            if (_r[i].colides(range)) {
                return true;
            }
        }

        return false;
    }
};

} // namespace Karm
