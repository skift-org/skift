#pragma once

#include "base.h"

namespace Karm::Scene {

struct Stack : public Node {
    Vec<Strong<Node>> _children;
    Opt<Math::Trans2f> _transform;

    Stack(Opt<Math::Trans2f> transform = NONE) : _transform(transform) {}

    void add(Strong<Node> child) {
        _children.pushBack(child);
    }

    void prepare() override {
        stableSort(_children, [](auto &a, auto &b) {
            return a->zIndex <=> b->zIndex;
        });

        for (auto &child : _children)
            child->prepare();
    }

    Math::Rectf bound() override {
        Math::Rectf rect;
        for (auto &child : _children)
            rect = rect.mergeWith(child->bound());
        return rect;
    }

    void paint(Gfx::Canvas &g, Math::Rectf r) override {
        if (not bound().colide(r))
            return;

        if (_transform) {
            g.push();
            g.transform(_transform.unwrap());
        }

        for (auto &child : _children)
            child->paint(g, r);

        if (_transform)
            g.pop();
    }

    void print(Print::Printer &p) override {
        for (auto &child : _children)
            child->print(p);
    }

    void repr(Io::Emit &e) const override {
        e("(stack z:{}", zIndex);
        if (_children) {
            e.indentNewline();
            for (auto &child : _children) {
                child->repr(e);
                e.newline();
            }
            e.deindent();
        }
        e(")");
    }
};

} // namespace Karm::Scene
