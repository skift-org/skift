#pragma once

#include <karm-base/try.h>
#include <karm-debug/logger.h>
#include <karm-gfx/path.h>

namespace Karm::Gfx {

struct Shape : public Vec<Math::Edgef> {
    Math::Rectf bound() const {
        Opt<Math::Rectf> bound{};
        for (auto const &edge : *this) {

            if (bound) {
                bound = bound->mergeWith(edge.bound());
            } else {
                bound = edge.bound();
            }
        }
        auto result = tryOr(bound, {}).cast<int>();
        return result.cast<double>();
    }
};

static void createSolid(Path &path, Shape &shape) {
    for (auto seg : path.iterSegs()) {
        if (seg.close) {
            for (size_t i = 0; i < seg.len() - 1; i++) {
                shape.pushBack(Math::Edgef{
                    seg[i],
                    seg[i + 1],
                });
            }
        }
    }
}

static void createStroke(Path &path, Shape &shape) {
    // TODO: stroke
    createSolid(path, shape);
}

} // namespace Karm::Gfx
