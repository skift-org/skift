#pragma once

#include <karm-base/range.h>
#include <karm-logger/logger.h>

#include "path.h"
#include "shape.h"

namespace Karm::Gfx {

enum struct FillRule {
    NONZERO,
    EVENODD,
};

struct Rast {
    static constexpr auto AA = 4;
    static constexpr auto UNIT = 1.0f / AA;
    static constexpr auto HALF_UNIT = 1.0f / AA / 2.0;

    struct Active {
        f64 x;
        isize sign;
    };

    struct Frag {
        Math::Vec2i xy;
        Math::Vec2f uv;
        f64 a;
    };

    Shape _shape{};
    Vec<Active> _active{};
    Vec<ISizeRange> _ranges;
    Vec<f64> _scanline{};

    Shape &shape() {
        return _shape;
    }

    void _appendRange(ISizeRange range) {
        usize i = 0;
        for (auto &r : _ranges) {
            if (r.overlaps(range)) {
                auto merged = r.merge(range);
                _ranges.removeAt(i);
                _appendRange(merged);
                return;
            }
            if (range.end() > r.start) {
                _ranges.insert(i, range);
                return;
            }
            i++;
        }

        _ranges.pushBack(range);
    }

    void clear() {
        _shape.clear();
    }

    void fill(Math::Recti clip, FillRule fillRule, auto cb) {
        auto shapeBound = _shape.bound().grow(0.3);
        auto rect = shapeBound
                        .ceil()
                        .cast<isize>()
                        .clipTo(clip);

        _scanline.resize(rect.width + 1);

        for (isize y = rect.top(); y < rect.bottom(); y++) {
            zeroFill<f64>(mutSub(_scanline, 0, rect.width + 1));
            _ranges.clear();

            for (f64 yy = y; yy < y + 1.0; yy += UNIT) {
                _active.clear();

                for (auto &edge : _shape) {
                    auto sample = yy + HALF_UNIT;

                    if (edge.bound().top() <= sample and sample < edge.bound().bottom()) {
                        _active.pushBack({
                            .x = edge.sx + (sample - edge.sy) / (edge.ey - edge.sy) * (edge.ex - edge.sx),
                            .sign = edge.sy > edge.ey ? 1 : -1,
                        });
                    }
                }

                if (_active.len() == 0)
                    continue;

                sort(_active, [](auto const &a, auto const &b) {
                    return a.x <=> b.x;
                });

                isize rule = 0;
                for (usize i = 0; i + 1 < _active.len(); i++) {
                    usize si = i;
                    usize ei = i + 1;

                    if (fillRule == FillRule::NONZERO) {
                        isize sign = _active[i].sign;
                        rule += sign;
                        if (rule == 0)
                            continue;
                    }

                    if (fillRule == FillRule::EVENODD) {
                        rule++;
                        if (rule % 2 == 0)
                            continue;
                    }

                    f64 x1 = max(_active[si].x, rect.start());
                    f64 x2 = min(_active[ei].x, rect.end());

                    if (x1 >= x2)
                        continue;

                    _appendRange(ISizeRange::fromStartEnd(floor(x1), ceil(x2)));

                    // Are x1 and x2 on the same pixel?
                    if (Math::floor(x1 - rect.x) == Math::floor(x2 - rect.x)) {
                        _scanline[Math::floor(x1 - rect.x)] += (x2 - x1) * UNIT;
                    } else {
                        _scanline[x1 - rect.x] += (ceil(x1) - x1) * UNIT;
                        _scanline[x2 - rect.x] += (x2 - floor(x2)) * UNIT;

                        for (isize x = ceil(x1); x < floor(x2); x++) {
                            _scanline[x - rect.x] += UNIT;
                        }
                    }
                }
            }

            for (auto r : _ranges) {
                for (isize x = r.start; x < r.end(); x++) {
                    auto xy = Math::Vec2i{x, y};

                    auto uv = Math::Vec2f{
                        (x - shapeBound.start()) / shapeBound.width,
                        (y - shapeBound.top()) / shapeBound.height,
                    };

                    cb(Frag{xy, uv, clamp01(_scanline[x - rect.x])});
                }
            }
        }
    }
};

} // namespace Karm::Gfx
