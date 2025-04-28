#pragma once

#include <karm-gfx/canvas.h>
#include <karm-io/emit.h>

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

    virtual void paint(Gfx::Canvas&, Math::Rectf = Math::Rectf::MAX, PaintOptions = {}) {}

    virtual void repr(Io::Emit& e) const {
        e("(node z:{})", zIndex);
    }
};

struct Proxy : public Node {
    Rc<Node> _node;

    Proxy(Rc<Node> node)
        : _node(node) {}

    void prepare() override {
        _node->prepare();
    }

    Math::Rectf bound() override {
        return _node->bound();
    }

    void paint(Gfx::Canvas& g, Math::Rectf r, PaintOptions o) override {
        _node->paint(g, r, o);
    }

    void repr(Io::Emit& e) const override {
        e("(proxy z:{} {})", zIndex, _node);
    }
};

struct Stack : Node {
    Vec<Rc<Node>> _children;

    void add(Rc<Node> child) {
        _children.pushBack(child);
    }

    void prepare() override {
        stableSort(_children, [](auto& a, auto& b) {
            return a->zIndex <=> b->zIndex;
        });

        for (auto& child : _children)
            child->prepare();
    }

    Math::Rectf bound() override {
        Math::Rectf rect;
        for (auto& child : _children)
            rect = rect.mergeWith(child->bound());

        return rect;
    }

    void paint(Gfx::Canvas& g, Math::Rectf r, PaintOptions o) override {
        if (not bound().colide(r))
            return;

        for (auto& child : _children)
            child->paint(g, r, o);
    }

    void repr(Io::Emit& e) const override {
        e("(stack z:{}", zIndex);
        if (_children) {
            e.indentNewline();
            for (auto& child : _children) {
                child->repr(e);
                e.newline();
            }
            e.deindent();
        }
        e(")");
    }
};

} // namespace Karm::Scene
