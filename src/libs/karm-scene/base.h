#pragma once

#include <karm-gfx/canvas.h>
#include <karm-io/emit.h>
#include <karm-print/base.h>

namespace Karm::Scene {

struct Node {
    isize zIndex = 0;

    virtual ~Node() = default;

    /// Prepare the scene graph for rendering (z-order, prunning, etc)
    virtual void prepare() {}

    /// The bounding rectangle of the node
    virtual Math::Recti bound() { return {}; }

    virtual void paint(Gfx::Canvas &) {}

    virtual void print(Print::Printer &) {}

    virtual void repr(Io::Emit &e) const {
        e("(node z:{})", zIndex);
    }
};

} // namespace Karm::Scene
