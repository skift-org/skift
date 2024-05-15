#pragma once

#include <karm-math/curve.h>
#include <karm-ui/box.h>
#include <karm-ui/drag.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/view.h>

#include "base.h"

namespace Hideo::Demos {

struct Edit {
    usize index;
    Math::Vec2f pos;
};

static void reduce(Math::Curvef &curve, Edit e) {
    curve._pts[e.index] = e.pos;
}

static inline Demo BEZIER_DEMO{
    Mdi::VECTOR_BEZIER,
    "Curve",
    "Curve editor",
    [] {
        using Model = Ui::Model<Math::Curvef, Edit, reduce>;
        return Ui::reducer<Model>(Math::Curvef::linear(0, 200), [](Math::Curvef const &s) {
            auto canvas = Ui::canvas(
                [=](Gfx::Context &g, Math::Vec2i) {
                    g.begin();
                    g.moveTo(s.a);
                    g.cubicTo(s.b, s.c, s.d);
                    g.strokeStyle(Gfx::stroke(Gfx::WHITE).withWidth(2).withAlign(Gfx::CENTER_ALIGN));
                    g.stroke();

                    auto minusOffset = s.offset(-50);
                    auto plusOffset = s.offset(50);

                    g.begin();
                    g.moveTo(minusOffset.a);
                    g.cubicTo(minusOffset.b, minusOffset.c, minusOffset.d);
                    g.strokeStyle(Gfx::stroke(Gfx::RED).withWidth(1).withAlign(Gfx::CENTER_ALIGN));
                    g.stroke();

                    g.begin();
                    g.moveTo(plusOffset.a);
                    g.cubicTo(plusOffset.b, plusOffset.c, plusOffset.d);
                    g.strokeStyle(Gfx::stroke(Gfx::GREEN).withWidth(1).withAlign(Gfx::CENTER_ALIGN));
                    g.stroke();

                    g.begin();
                    g.moveTo(s.a);
                    g.lineTo(s.b);
                    g.lineTo(s.c);
                    g.lineTo(s.d);
                    g.strokeStyle(Gfx::stroke(Gfx::WHITE).withWidth(0.5).withAlign(Gfx::CENTER_ALIGN));
                    g.stroke();
                }
            );

            auto pointHandle = [](usize index, Math::Vec2f pos) {
                return Ui::empty(8) |
                       Ui::box(Ui::BoxStyle{
                           .borderRadius = 16,
                           .backgroundPaint = Gfx::WHITE,
                       }) |
                       Ui::center() |
                       Ui::bound() |
                       Ui::dragRegion() |
                       Ui::placed(Math::Recti::fromCenter(pos.cast<isize>(), {12, 12})) |
                       Ui::intent([=](Ui::Node &n, Sys::Event &e) {
                           if (auto *m = e.is<Ui::DragEvent>(Sys::Propagation::UP)) {
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
