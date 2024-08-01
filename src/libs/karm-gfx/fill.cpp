#include "fill.h"

namespace Karm::Gfx {

static Color _lerp(Gradient::Stop lhs, Gradient::Stop rhs, f64 pos) {
    f64 t = (pos - lhs.cdr) / (rhs.cdr - lhs.cdr);
    return lhs.car.lerpWith(rhs.car, t);
}

static void _bakeStops(Slice<Gradient::Stop> stops, Gradient::Buf &buf, bool wraparound) {
    fill(mutSub(buf), Gfx::BLACK);

    if (stops.len() == 0)
        return;

    if (stops.len() == 1) {
        fill(mutSub(buf), stops[0].car);
        return;
    }

    for (float j = 0; j < stops[0].cdr * 256; j++) {
        if (wraparound) {
            auto lhs = stops[stops.len() - 1];
            lhs.cdr -= 1;
            buf[j] = _lerp(lhs, stops[0], j / 256.0);
        } else {
            buf[j] = stops[0].car;
        }
    }

    for (usize i = 0; i < stops.len() - 1; i++) {
        auto iPos = stops[i].cdr;
        auto jPos = stops[i + 1].cdr;

        for (f64 j = iPos * 256; j < jPos * 256; j++)
            buf[j] = _lerp(stops[i], stops[i + 1], j / 256.0);
    }

    for (f64 j = last(stops).cdr * 256; j < 256; j++) {
        if (wraparound) {
            auto rhs = stops[0];
            rhs.cdr += 1;
            buf[j] = _lerp(last(stops), rhs, j / 256.0);
        } else {
            buf[j] = last(stops).car;
        }
    }
}

Gradient Gradient::Builder::bake() {
    auto buf = makeStrong<Buf>();
    _bakeStops(_stops, *buf, _type == CONICAL);
    return {_type, _start, _end, buf};
}

} // namespace Karm::Gfx
