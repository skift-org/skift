#pragma once

// https://web.archive.org/web/20050408192410/http://sw-shader.sourceforge.net/rasterizer.html

#include <karm-image/loader.h>
#include <karm-math/edge.h>
#include <karm-math/mesh.h>
#include <karm-ui/box.h>
#include <karm-ui/drag.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/view.h>

#include "base.h"

namespace Hideo::Demos {

static void reduce(Math::Mesh2f &curve, Edit e) {
    curve.verts[e.index].xy = e.pos;
}

Math::Vec3f barycentric(Math::Vec2f a, Math::Vec2f b, Math::Vec2f c, Math::Vec2i xy) {
    auto v0 = b - a;
    auto v1 = c - a;
    auto v2 = xy.cast<float>() - a;
    auto d00 = v0.dot(v0);
    auto d01 = v0.dot(v1);
    auto d11 = v1.dot(v1);
    auto d20 = v2.dot(v0);
    auto d21 = v2.dot(v1);
    auto denom = d00 * d11 - d01 * d01;
    auto v = (d11 * d20 - d01 * d21) / denom;
    auto w = (d00 * d21 - d01 * d20) / denom;
    auto u = 1.0f - v - w;
    return {u, v, w};
}

void fillTri(Math::Vert2f a, Math::Vert2f b, Math::Vert2f c, Gfx::Fill fill, Gfx::MutPixels buf) {
    Math::Vec2i min = a.xy.min(b.xy).min(c.xy).cast<isize>();
    Math::Vec2i max = a.xy.max(b.xy).max(c.xy).cast<isize>();

    for (isize x = min.x; x <= max.x; x++) {
        for (isize y = min.y; y <= max.y; y++) {
            auto bc = barycentric(a.xy, b.xy, c.xy, {x, y});
            auto uv = a.uv * bc.x + b.uv * bc.y + c.uv * bc.z;
            if (bc.x < 0 || bc.y < 0 || bc.z < 0)
                continue;
            buf.store({x, y}, fill.sample(uv));
        }
    }
}

static inline Demo MESH_DEMO{
    Mdi::VECTOR_SQUARE_EDIT,
    "Mesh",
    "Mesh editor",
    [] {
        using Model = Ui::Model<Math::Mesh2f, Edit, reduce>;
        auto mesh = Math::Mesh2f::plane(300, 300, 3, 3);
        mesh.offset(200.0);
        return Ui::reducer<Model>(mesh, [](Math::Mesh2f const &s) {
            auto canvas = Ui::canvas(
                [=, image = Image::loadOrFallback("file:./src/specs/qoi/tests/res/kodim23.qoi"_url).unwrap()](Gfx::Context &g, Math::Vec2i) {
                    Gfx::Fill fill = image.pixels();

                    for (usize i = 0; i + 2 < s.index.len(); i += 3) {
                        auto a = s.verts[s.index[i]];
                        a.xy = g.current().trans.apply(a.xy);
                        auto b = s.verts[s.index[i + 1]];
                        b.xy = g.current().trans.apply(b.xy);
                        auto c = s.verts[s.index[i + 2]];
                        c.xy = g.current().trans.apply(c.xy);
                        fillTri(a, b, c, fill, *g._pixels);
                    }

                    g.strokeStyle(Gfx::stroke(Gfx::WHITE).withWidth(1).withAlign(Gfx::OUTSIDE_ALIGN));

                    for (usize i = 0; i + 2 < s.index.len(); i += 3) {
                        g.begin();
                        g.moveTo(s.verts[s.index[i]].xy);
                        g.lineTo(s.verts[s.index[i + 1]].xy);
                        g.lineTo(s.verts[s.index[i + 2]].xy);
                        g.close();
                        g.stroke();
                    }
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
                           if (auto *m = e.is<Ui::DragEvent>()) {
                               e.accept();
                               Model::bubble<Edit>(n, {index, pos + m->delta});
                           }
                       });
            };

            Ui::Children handles;
            handles.pushBack(canvas);
            for (usize i = 0; i < s.verts.len(); i++) {
                handles.pushBack(pointHandle(i, s.verts[i].xy));
            }

            return Ui::stack(handles);
        });
    },
};

} // namespace Hideo::Demos
