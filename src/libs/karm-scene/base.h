#pragma once

#include <karm-gfx/canvas.h>
#include <karm-io/emit.h>
#include <karm-print/printer.h>

namespace Karm::Scene {

struct PaintOptions {
    bool showBackgroundGraphics = true;
};

struct Node {
    isize zIndex = 0;

    virtual ~Node() = default;

    /// Prepare the scene graph for rendering (z-order, prunning, etc)
    virtual void prepare() {}

    /// The bounding rectangle of the node
    virtual Math::Rectf bound() { return {}; }

    virtual void paint(Gfx::Canvas &, Math::Rectf = Math::Rectf::MAX, PaintOptions = {}) {}

    virtual void print(Print::Printer &, PaintOptions = {.showBackgroundGraphics = false}) {}

    virtual void repr(Io::Emit &e) const {
        e("(node z:{})", zIndex);
    }
};

} // namespace Karm::Scene
