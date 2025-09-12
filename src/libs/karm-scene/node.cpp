module;

#include <karm-gfx/cpu/canvas.h>

export module Karm.Scene:node;

import Karm.Core;

namespace Karm::Scene {

export struct PaintOptions {
    bool showBackgroundGraphics = true;
};

export struct Node {
    isize zIndex = 0;

    virtual ~Node() = default;

    /// Prepare the scene graph for rendering (z-order, prunning, etc...)
    virtual void prepare() {}

    /// The bounding rectangle of the node
    virtual Math::Rectf bound() { return {}; }

    virtual void paint(Gfx::Canvas&, Math::Rectf, PaintOptions = {}) {}

    virtual void repr(Io::Emit& e) const {
        e("(node z:{})", zIndex);
    }

    Rc<Gfx::Surface> snapshot(Math::Vec2f size, f64 density = 1) {
        auto surface = Gfx::Surface::alloc(
            size.ceil().cast<isize>() * density,
            Gfx::RGBA8888
        );

        Gfx::CpuCanvas g;
        g.begin(*surface);
        g.scale(density);
        paint(g, Math::Rectf{size});
        g.end();

        return surface;
    }
};

} // namespace Karm::Scene
