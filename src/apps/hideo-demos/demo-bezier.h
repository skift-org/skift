#pragma once

#include <karm-math/curve.h>
#include <karm-ui/box.h>
#include <karm-ui/drag.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/view.h>
#include <mdi/vector-bezier.h>

#include "base.h"

namespace Hideo::Demos {

static Ui::Task<Edit> reduce(Math::Curvef &curve, Edit e) {
    curve._pts[e.index] = e.pos;
    return NONE;
}

static inline Demo BEZIER_DEMO{
    Mdi::VECTOR_BEZIER,
    "Curve",
    "Curve editor",
    [] {
        using Model = Ui::Model<Math::Curvef, Edit, reduce>;
        return Ui::reducer<Model>(Math::Curvef::linear(100, 300), [](Math::Curvef const &s) {
            auto canvas = Ui::canvas(
                [=](Gfx::Canvas &g, Math::Vec2i) {
                    g.beginPath();
                    g.moveTo(s.a);
                    g.cubicTo(s.b, s.c, s.d);

                    g.strokeStyle({
                        .fill = Gfx::WHITE,
                        .width = 2,
                        .align = Gfx::CENTER_ALIGN,
                    });

                    g.stroke();

                    auto minusOffset = s.offset2(-50);
                    auto plusOffset = s.offset(-50);

                    g.beginPath();
                    g.moveTo(minusOffset.a);
                    g.cubicTo(minusOffset.b, minusOffset.c, minusOffset.d);
                    g.strokeStyle(Gfx::stroke(Gfx::RED).withWidth(1).withAlign(Gfx::CENTER_ALIGN));
                    g.stroke();

                    g.beginPath();
                    g.moveTo(minusOffset.a);
                    g.lineTo(minusOffset.b);
                    g.lineTo(minusOffset.c);
                    g.lineTo(minusOffset.d);
                    g.strokeStyle(Gfx::stroke(Gfx::RED).withWidth(0.5).withAlign(Gfx::CENTER_ALIGN));
                    g.stroke();

                    g.beginPath();
                    g.moveTo(plusOffset.a);
                    g.cubicTo(plusOffset.b, plusOffset.c, plusOffset.d);
                    g.strokeStyle(Gfx::stroke(Gfx::GREEN).withWidth(1).withAlign(Gfx::CENTER_ALIGN));
                    g.stroke();

                    g.beginPath();
                    g.moveTo(plusOffset.a);
                    g.lineTo(plusOffset.b);
                    g.lineTo(plusOffset.c);
                    g.lineTo(plusOffset.d);
                    g.strokeStyle(Gfx::stroke(Gfx::GREEN).withWidth(0.5).withAlign(Gfx::CENTER_ALIGN));
                    g.stroke();

                    g.beginPath();
                    g.moveTo(s.a);
                    g.lineTo(s.b);
                    g.strokeStyle(Gfx::stroke(Gfx::WHITE).withWidth(0.5).withAlign(Gfx::CENTER_ALIGN));
                    g.stroke();

                    g.beginPath();
                    g.moveTo(s.d);
                    g.lineTo(s.c);
                    g.strokeStyle(Gfx::stroke(Gfx::WHITE).withWidth(0.5).withAlign(Gfx::CENTER_ALIGN));
                    g.stroke();
                }
            );

            auto pointHandle = [](usize index, Math::Vec2f pos) {
                return Ui::empty(8) |
                       Ui::box(Ui::BoxStyle{
                           .borderRadii = 16,
                           .backgroundFill = Gfx::WHITE,
                       }) |
                       Ui::center() |
                       Ui::bound() |
                       Ui::dragRegion() |
                       Ui::placed(Math::Recti::fromCenter(pos.cast<isize>(), {24, 24})) |
                       Ui::intent([=](Ui::Node &n, App::Event &e) {
                           if (auto m = e.is<Ui::DragEvent>()) {
                               e.accept();
                               Model::bubble<Edit>(n, {index, pos + m->delta});
                           }
                       });
            };

            return Ui::stack(
                canvas,
                pointHandle(0, s.a),
                pointHandle(1, s.b),
                pointHandle(2, s.c),
                pointHandle(3, s.d)
            );
        });
    },
};

} // namespace Hideo::Demos
