#pragma once

#include <karm-gfx/canvas.h>
#include <karm-gfx/fill.h>
#include <karm-math/curve.h>
#include <karm-math/radii.h>
#include <vaev-base/borders.h>

#include "base.h"

namespace Vaev::Paint {

struct BorderPaint {
    f64 width;
    BorderStyle style;
    Gfx::Color fill = Gfx::BLACK;
};

// https://drafts.csswg.org/css-backgrounds/#borders
struct Borders : public Node {
    Math::Rectf bound;
    BorderCollapse collapse;
    Math::Radiif radii;

    BorderPaint top = {
        0,
        BorderStyle::SOLID,
        BLACK,
    };

    BorderPaint start = {
        0,
        BorderStyle::SOLID,
        BLACK,
    };

    BorderPaint bottom = {
        0,
        BorderStyle::SOLID,
        BLACK,
    };

    BorderPaint end = {
        0,
        BorderStyle::SOLID,
        BLACK,
    };

    constexpr void all(BorderPaint b) {
        top = start = bottom = end = b;
    }

    constexpr void set(BorderEdge edge, BorderPaint b) {
        switch (edge) {
        case BorderEdge::ALL:
            all(b);
            break;
        case BorderEdge::TOP:
            top = b;
            break;
        case BorderEdge::START:
            start = b;
            break;
        case BorderEdge::BOTTOM:
            bottom = b;
            break;
        case BorderEdge::END:
            end = b;
            break;
        }
    }

    void paint(Gfx::Canvas &ctx) override {
        ctx.beginPath();

        Math::Vec2f const cornerTopStart = {bound.x - start.width, bound.y - top.width};
        auto const radiiIn = computeInternalRadii(radii);

        if (top.fill == start.fill and
            start.fill == bottom.fill and
            bottom.fill == end.fill and
            top.style == bottom.style and
            bottom.style == start.style and
            start.style == end.style) {
            // fast path, this allow us to paint the border easily if all the borders are the same color/style
            Math::Rectf const outer = Math::Rectf{
                cornerTopStart,
                {
                    bound.width + end.width + start.width,
                    bound.height + top.width + bottom.width,
                }
            };

            ctx.rect(outer, radii);
            ctx.rect(bound, radiiIn);
            ctx.fill(top.fill, Gfx::FillRule::EVENODD);
            return;
        }

        // precise path, allow us to draw each border independantly
        auto outerCurves = computeOuterCurves(cornerTopStart, bound);
        auto innerCurves = computeInnerCurves(cornerTopStart, bound, radiiIn);

        paintEdge(ctx, BorderEdge::TOP, outerCurves, innerCurves);
        paintEdge(ctx, BorderEdge::BOTTOM, outerCurves, innerCurves);
        paintEdge(ctx, BorderEdge::START, outerCurves, innerCurves);
        paintEdge(ctx, BorderEdge::END, outerCurves, innerCurves);
    }

    Array<Math::Curvef, 8> computeOuterCurves(Math::Vec2f topStart, Math::Rectf content) {
        Array<Math::Curvef, 8> curves = {};

        // corner top-end
        auto startingpoint = Math::Vec2f{
            topStart.x + start.width + content.width + end.width - radii.c,
            topStart.y
        };
        auto endpoint = Math::Vec2f{startingpoint.x + radii.c, startingpoint.y + radii.d};
        f64 cp1 = radii.c * 0.5522847498;
        f64 cp2 = radii.d * 0.5522847498;

        auto curve = Math::Curvef::cubic(
            startingpoint,
            {startingpoint.x + cp1, startingpoint.y},
            {endpoint.x, endpoint.y - cp2},
            endpoint
        );

        f64 ratio;
        if (top.width == 0 and end.width == 0) {
            ratio = 0.5;
        } else {
            ratio = top.width / (end.width + top.width);
        }
        auto [part1, part2] = curve.split(ratio);
        curves[0] = part1;
        curves[1] = part2;

        // corner bottom-end
        startingpoint = Math::Vec2f{
            topStart.x + start.width + content.width + end.width,
            topStart.y + top.width + content.height + bottom.width - radii.e
        };
        endpoint = Math::Vec2f{startingpoint.x - radii.f, startingpoint.y + radii.e};
        cp1 = radii.e * 0.5522847498;
        cp2 = radii.f * 0.5522847498;

        curve = Math::Curvef::cubic(
            startingpoint,
            {startingpoint.x, startingpoint.y + cp1},
            {endpoint.x + cp2, endpoint.y},
            endpoint
        );

        if (end.width == 0 and bottom.width == 0) {
            ratio = 0.5;
        } else {
            ratio = end.width / (end.width + bottom.width);
        }
        auto [part3, part4] = curve.split(ratio);
        curves[2] = part3;
        curves[3] = part4;

        // corner bottom-start
        startingpoint = Math::Vec2f{
            topStart.x + radii.g,
            topStart.y + content.height + top.width + bottom.width,
        };
        endpoint = Math::Vec2f{topStart.x, startingpoint.y - radii.h};
        cp1 = radii.g * 0.5522847498;
        cp2 = radii.h * 0.5522847498;

        curve = Math::Curvef::cubic(
            startingpoint,
            {startingpoint.x - cp1, startingpoint.y},
            {endpoint.x, endpoint.y + cp2},
            endpoint
        );

        if (bottom.width == 0 and start.width == 0) {
            ratio = 0.5;
        } else {
            ratio = bottom.width / (start.width + bottom.width);
        }
        auto [part5, part6] = curve.split(ratio);
        curves[4] = part5;
        curves[5] = part6;

        // corner top-start
        startingpoint = Math::Vec2f{topStart.x, topStart.y + radii.a};
        endpoint = Math::Vec2f{topStart.x + radii.b, topStart.y};
        cp1 = radii.a * 0.5522847498;
        cp2 = radii.b * 0.5522847498;

        curve = Math::Curvef::cubic(
            startingpoint,
            {startingpoint.x, startingpoint.y - cp1},
            {endpoint.x - cp2, endpoint.y},
            endpoint
        );

        if (top.width == 0 and start.width == 0) {
            ratio = 0.5;
        } else {
            ratio = start.width / (top.width + start.width);
        }
        auto [part7, part8] = curve.split(ratio);
        curves[6] = part7;
        curves[7] = part8;
        return curves;
    }

    Array<Math::Curvef, 8> computeInnerCurves(Math::Vec2f topStart, Math::Rectf content, Math::Radiif const &radii) {
        Array<Math::Curvef, 8> curves = {};

        // corner top-end
        auto startingpoint = Math::Vec2f{
            topStart.x + start.width + content.width - radii.c,
            topStart.y + top.width
        };
        auto endpoint = Math::Vec2f{startingpoint.x + radii.c, startingpoint.y + radii.d};
        f64 cp1 = radii.c * 0.5522847498;
        f64 cp2 = radii.d * 0.5522847498;

        auto curve = Math::Curvef::cubic(
            startingpoint,
            {startingpoint.x + cp1, startingpoint.y},
            {endpoint.x, endpoint.y - cp2},
            endpoint
        );

        f64 ratio;
        if (end.width == 0 and top.width == 0) {
            ratio = 0.5;
        } else {
            ratio = top.width / (end.width + top.width);
        }
        auto [part1, part2] = curve.split(ratio);
        curves[0] = part1;
        curves[1] = part2;

        // corner bottom-end
        startingpoint = Math::Vec2f{
            topStart.x + start.width + content.width,
            topStart.y + top.width + content.height - radii.e
        };
        endpoint = Math::Vec2f{startingpoint.x - radii.f, startingpoint.y + radii.e};
        cp1 = radii.e * 0.5522847498;
        cp2 = radii.f * 0.5522847498;

        curve = Math::Curvef::cubic(
            startingpoint,
            {startingpoint.x, startingpoint.y + cp1},
            {endpoint.x + cp2, endpoint.y},
            endpoint
        );

        if (end.width == 0 and bottom.width == 0) {
            ratio = 0.5;
        } else {
            ratio = end.width / (end.width + bottom.width);
        }
        auto [part3, part4] = curve.split(ratio);
        curves[2] = part3;
        curves[3] = part4;

        // corner bottom-start
        startingpoint = Math::Vec2f{
            topStart.x + start.width + radii.g,
            topStart.y + content.height + top.width,
        };
        endpoint = Math::Vec2f{startingpoint.x - radii.g, startingpoint.y - radii.h};
        cp1 = radii.g * 0.5522847498;
        cp2 = radii.h * 0.5522847498;

        curve = Math::Curvef::cubic(
            startingpoint,
            {startingpoint.x - cp1, startingpoint.y},
            {endpoint.x, endpoint.y + cp2},
            endpoint
        );

        if (bottom.width == 0 and start.width == 0) {
            ratio = 0.5;
        } else {
            ratio = bottom.width / (start.width + bottom.width);
        }
        auto [part5, part6] = curve.split(ratio);
        curves[4] = part5;
        curves[5] = part6;

        // corner top-start
        startingpoint = Math::Vec2f{topStart.x + start.width, topStart.y + top.width + radii.a};
        endpoint = Math::Vec2f{startingpoint.x + radii.b, topStart.y + top.width};
        cp1 = radii.a * 0.5522847498;
        cp2 = radii.b * 0.5522847498;

        curve = Math::Curvef::cubic(
            startingpoint,
            {startingpoint.x, startingpoint.y - cp1},
            {endpoint.x - cp2, endpoint.y},
            endpoint
        );

        if (top.width == 0 and start.width == 0) {
            ratio = 0.5;
        } else {
            ratio = start.width / (top.width + start.width);
        }
        auto [part7, part8] = curve.split(ratio);
        curves[6] = part7;
        curves[7] = part8;
        return curves;
    }

    void paintEdge(Gfx::Canvas &ctx, BorderEdge border, Array<Math::Curvef, 8> curvesOUT, Array<Math::Curvef, 8> curvesIN) {
        ctx.beginPath();

        if (border == BorderEdge::TOP) {
            ctx.moveTo(curvesOUT[7].a);
            ctx.cubicTo(
                curvesOUT[7].b,
                curvesOUT[7].c,
                curvesOUT[7].d
            );

            ctx.lineTo(curvesOUT[0].a);
            ctx.cubicTo(
                curvesOUT[0].b,
                curvesOUT[0].c,
                curvesOUT[0].d
            );

            ctx.lineTo(curvesIN[0].d);
            ctx.cubicTo(
                curvesIN[0].c,
                curvesIN[0].b,
                curvesIN[0].a
            );

            ctx.lineTo(curvesIN[7].d);
            ctx.cubicTo(
                curvesIN[7].c,
                curvesIN[7].b,
                curvesIN[7].a
            );
            ctx.closePath();
            ctx.fill(top.fill);
        } else if (border == BorderEdge::END) {
            ctx.moveTo(curvesOUT[1].a);
            ctx.cubicTo(
                curvesOUT[1].b,
                curvesOUT[1].c,
                curvesOUT[1].d
            );

            ctx.lineTo(curvesOUT[2].a);
            ctx.cubicTo(
                curvesOUT[2].b,
                curvesOUT[2].c,
                curvesOUT[2].d
            );

            ctx.lineTo(curvesIN[2].d);
            ctx.cubicTo(
                curvesIN[2].c,
                curvesIN[2].b,
                curvesIN[2].a
            );

            ctx.lineTo(curvesIN[1].d);
            ctx.cubicTo(
                curvesIN[1].c,
                curvesIN[1].b,
                curvesIN[1].a
            );
            ctx.closePath();
            ctx.fill(end.fill);
        } else if (border == BorderEdge::BOTTOM) {
            ctx.moveTo(curvesOUT[3].a);
            ctx.cubicTo(
                curvesOUT[3].b,
                curvesOUT[3].c,
                curvesOUT[3].d
            );

            ctx.lineTo(curvesOUT[4].a);
            ctx.cubicTo(
                curvesOUT[4].b,
                curvesOUT[4].c,
                curvesOUT[4].d
            );

            ctx.lineTo(curvesIN[4].d);
            ctx.cubicTo(
                curvesIN[4].c,
                curvesIN[4].b,
                curvesIN[4].a
            );

            ctx.lineTo(curvesIN[3].d);
            ctx.cubicTo(
                curvesIN[3].c,
                curvesIN[3].b,
                curvesIN[3].a
            );

            ctx.closePath();
            ctx.fill(bottom.fill);
        } else if (border == BorderEdge::START) {
            ctx.moveTo(curvesOUT[5].a);
            ctx.cubicTo(
                curvesOUT[5].b,
                curvesOUT[5].c,
                curvesOUT[5].d
            );

            ctx.lineTo(curvesOUT[6].a);
            ctx.cubicTo(
                curvesOUT[6].b,
                curvesOUT[6].c,
                curvesOUT[6].d
            );

            ctx.lineTo(curvesIN[6].d);
            ctx.cubicTo(
                curvesIN[6].c,
                curvesIN[6].b,
                curvesIN[6].a
            );

            ctx.lineTo(curvesIN[5].d);
            ctx.cubicTo(
                curvesIN[5].c,
                curvesIN[5].b,
                curvesIN[5].a
            );

            ctx.closePath();
            ctx.fill(start.fill);
        }
    }

    Math::Radiif computeInternalRadii(Math::Radiif radii) {
        if (radii.a >= top.width) {
            radii.a = radii.a - top.width;
        }
        if (radii.b >= start.width) {
            radii.b = radii.b - start.width;
        }
        if (radii.c >= end.width) {
            radii.c = radii.c - end.width;
        }
        if (radii.d >= top.width) {
            radii.d = radii.d - top.width;
        }
        if (radii.e >= bottom.width) {
            radii.e = radii.e - bottom.width;
        }
        if (radii.f >= end.width) {
            radii.f = radii.f - end.width;
        }
        if (radii.g >= start.width) {
            radii.g = radii.g - start.width;
        }
        if (radii.h >= bottom.width) {
            radii.h = radii.h - bottom.width;
        }

        return radii;
    };
};

} // namespace Vaev::Paint
