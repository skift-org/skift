#pragma once

#include "canvas.h"

namespace Karm::Gfx {

struct BoxShadow {
    Gfx::Color fill;
    f64 blur;
    f64 spread;
    Math::Vec2i offset;

    static BoxShadow elevated(f64 v, Gfx::Color fill = Gfx::BLACK) {
        return {
            fill.withOpacity(0.25),
            v * 2,
            -v,
            {0, (int)v},
        };
    }

    auto &withFill(Gfx::Color p) {
        fill = p;
        return *this;
    }

    auto &withBlur(f64 r) {
        blur = r;
        return *this;
    }

    auto &withSpread(f64 r) {
        spread = r;
        return *this;
    }

    auto &withOffset(Math::Vec2i o) {
        offset = o;
        return *this;
    }

    void paint(Gfx::Canvas &g, Math::Recti bound) const {
        /// 1 / sqrt(2)
        static constexpr f64 IS2 = 0.7071067811865475;

        // 1 - (1 / sqrt(2))
        static constexpr f64 IS2M = 1 - IS2;

        bound = bound.grow(spread);
        bound = bound.offset(offset);

        auto grad = Gfx::Gradient::linear().withColors(fill, fill.withOpacity(0)).bake();

        auto topStart = Math::Recti::fromTwoPoint(
            bound.topStart(),
            bound.topStart() - Math::Vec2i{(int)blur, (int)blur}
        );

        auto topEnd = Math::Recti::fromTwoPoint(
            bound.topEnd(),
            bound.topEnd() + Math::Vec2i{(int)blur, -(int)blur}
        );

        auto bottomStart = Math::Recti::fromTwoPoint(
            bound.bottomStart(),
            bound.bottomStart() + Math::Vec2i{-(int)blur, (int)blur}
        );

        auto bottomEnd = Math::Recti::fromTwoPoint(
            bound.bottomEnd(),
            bound.bottomEnd() + Math::Vec2i{(int)blur, (int)blur}
        );

        auto top = Math::Recti::fromTwoPoint(
            bound.topStart() - Math::Vec2i{0, (int)blur},
            bound.topEnd()
        );

        auto bottom = Math::Recti::fromTwoPoint(
            bound.bottomStart(),
            bound.bottomEnd() + Math::Vec2i{0, (int)blur}
        );

        auto start = Math::Recti::fromTwoPoint(
            bound.topStart() - Math::Vec2i{(int)blur, 0},
            bound.bottomStart()
        );

        auto end = Math::Recti::fromTwoPoint(
            bound.topEnd(),
            bound.bottomEnd() + Math::Vec2i{(int)blur, 0}
        );

        grad.withType(Gradient::RADIAL);
        g.fillStyle(grad.withPoints({1, 1}, {IS2M, IS2M}));
        g.fill(topStart);

        g.fillStyle(grad.withPoints({0, 1}, {IS2, IS2M}));
        g.fill(topEnd);

        g.fillStyle(grad.withPoints({1, 0}, {IS2M, IS2}));
        g.fill(bottomStart);

        g.fillStyle(grad.withPoints({0, 0}, {IS2, IS2}));
        g.fill(bottomEnd);

        grad.withType(Gradient::LINEAR);
        g.fillStyle(grad.withPoints({0, 1}, {0, 0}));
        g.fill(top);

        g.fillStyle(grad.withPoints({0, 0}, {0, 1}));
        g.fill(bottom);

        g.fillStyle(grad.withPoints({1, 0}, {0, 0}));
        g.fill(start);

        g.fillStyle(grad.withPoints({0, 0}, {1, 0}));
        g.fill(end);

        g.fillStyle(fill);
        g.fill(bound);
    }
};

Gfx::BoxShadow boxShadow(auto... args) {
    return Gfx::BoxShadow(args...);
}

} // namespace Karm::Gfx
