#pragma once

#include <karm-base/range.h>
#include <karm-math/poly.h>

#include "../types.h"

namespace Karm::Gfx {

struct CpuRast {
    static constexpr auto AA = 3;
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

    Vec<Active> _active{};
    Vec<irange> _ranges;
    Vec<f64> _scanline{};

    void _appendRange(irange range) {
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

    void fill(Math::Polyf &poly, Math::Recti clip, FillRule fillRule, auto cb) {
        auto polyBound = poly.bound().grow(UNIT);
        auto clipBound = polyBound
                             .ceil()
                             .cast<isize>()
                             .clipTo(clip);

        _scanline.resize(clipBound.width + 1);

        for (isize y = clipBound.top(); y < clipBound.bottom(); y++) {
            zeroFill<f64>(mutSub(_scanline, 0, clipBound.width + 1));
            _ranges.clear();

            for (f64 yy = y; yy < y + 1.0; yy += UNIT) {
                _active.clear();

                for (auto &edge : poly) {
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
                    if (fillRule == FillRule::NONZERO) {
                        rule += _active[i].sign;
                        if (rule == 0)
                            continue;
                    }

                    if (fillRule == FillRule::EVENODD) {
                        rule++;
                        if (rule % 2 == 0)
                            continue;
                    }

                    // Clip the range to the clip rect
                    // NOTE: The following looks a bit verbose but it's necessary to avoid
                    //       floating point errors when converting to integer coordinates.
                    f64 x1 = max(_active[i].x, clipBound.start());

                    isize cx1 = max(Math::ceili(_active[i].x), clipBound.start());
                    isize fx1 = max(Math::floori(_active[i].x), clipBound.start());

                    f64 x2 = min(_active[i + 1].x, clipBound.end());
                    isize cx2 = min(Math::ceili(_active[i + 1].x), clipBound.end());
                    isize fx2 = min(Math::floori(_active[i + 1].x), clipBound.end());

                    // Skip if the range is empty
                    if (x1 >= x2)
                        continue;

                    _appendRange(irange::fromStartEnd(fx1, cx2));

                    // Are x1 and x2 on the same pixel?
                    if (fx1 == fx2) {
                        _scanline[fx1 - clipBound.x] += (x2 - x1) * UNIT;
                        continue;
                    }

                    // Compute the coverage for the first and last pixel
                    _scanline[x1 - clipBound.x] += (cx1 - x1) * UNIT;
                    _scanline[x2 - clipBound.x] += (x2 - fx2) * UNIT;

                    // Fill the pixels in between
                    for (isize x = cx1; x < fx2; x++)
                        _scanline[x - clipBound.x] += UNIT;
                }
            }

            for (auto r : _ranges) {
                for (isize x = r.start; x < r.end(); x++) {
                    auto xy = Math::Vec2i{x, y};

                    auto uv = Math::Vec2f{
                        (x - polyBound.start()) / polyBound.width,
                        (y - polyBound.top()) / polyBound.height,
                    };

                    cb(Frag{xy, uv, clamp01(_scanline[x - clipBound.x])});
                }
            }
        }
    }
};

} // namespace Karm::Gfx
