#pragma once

#include <karm-gfx/context.h>
#include <karm-print/context.h>

namespace Web::Paint {

struct Node {
    isize zIndex = 0;

    virtual ~Node() = default;

    /// Prepare the scene graph for rendering (z-order, prunning, etc)
    virtual void prepare() {}

    /// The bounding rectangle of the node
    virtual Math::Recti bound() { return {}; }

    virtual void paint(Gfx::Context &) {}

    virtual void print(Print::Context &) {}
};

} // namespace Web::Paint
