#include "fill.h"

namespace Karm::Gfx {

static Color _lerp(Gradient::Stop lhs, Gradient::Stop rhs, f64 pos) {
    f64 t = (pos - lhs.v1) / (rhs.v1 - lhs.v1);
    return lhs.v0.lerpWith(rhs.v0, t);
}

static void _bakeStops(Slice<Gradient::Stop> stops, Gradient::Buf& buf, bool wraparound) {
    fill(mutSub(buf), Gfx::BLACK);

    if (stops.len() == 0)
        return;

    if (stops.len() == 1) {
        fill(mutSub(buf), stops[0].v0);
        return;
    }

    for (float j = 0; j < stops[0].v1 * 256; j++) {
        if (wraparound) {
            auto lhs = stops[stops.len() - 1];
            lhs.v1 -= 1;
            buf[j] = _lerp(lhs, stops[0], j / 256.0);
        } else {
            buf[j] = stops[0].v0;
        }
    }

    for (usize i = 0; i < stops.len() - 1; i++) {
        auto iPos = stops[i].v1;
        auto jPos = stops[i + 1].v1;

        for (f64 j = iPos * 256; j < jPos * 256; j++)
            buf[j] = _lerp(stops[i], stops[i + 1], j / 256.0);
    }

    for (f64 j = last(stops).v1 * 256; j < 256; j++) {
        if (wraparound) {
            auto rhs = stops[0];
            rhs.v1 += 1;
            buf[j] = _lerp(last(stops), rhs, j / 256.0);
        } else {
            buf[j] = last(stops).v0;
        }
    }
}

Gradient Gradient::Builder::bake() {
    auto buf = makeRc<Buf>();
    _bakeStops(_stops, *buf, _type == CONICAL);
    return {_type, _start, _end, buf};
}

} // namespace Karm::Gfx
