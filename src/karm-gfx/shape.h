#pragma once

#include <karm-gfx/path.h>

namespace Karm::Gfx {

using Shape = Vec<Math::Edgef>;

static void createSolid(Path &path, Shape &shape) {
    for (auto seg : path.iterSegs()) {
        if (!seg.close)
            for (size_t i = 0; i < seg.len() - 1; i++) {
                shape.pushBack(Math::Edgef{
                    seg[i],
                    seg[i + 1],
                });
            }
    }
}

static void createStroke(Path &path, Shape &shape) {
    // TODO: stroke
    createSolid(path, shape);
}

} // namespace Karm::Gfx
