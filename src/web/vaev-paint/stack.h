#pragma once

#include "base.h"

namespace Vaev::Paint {

struct Stack : public Node {
    Vec<Strong<Node>> _children;

    void add(Strong<Node> child) {
        _children.pushBack(child);
    }

    void prepare() override {
        stableSort(_children, [](auto &a, auto &b) {
            return a->zIndex < b->zIndex;
        });

        for (auto &child : _children)
            child->prepare();
    }

    Math::Recti bound() override {
        Math::Recti rect;
        for (auto &child : _children)
            rect = rect.mergeWith(child->bound());
        return rect;
    }

    void paint(Gfx::Context &ctx) override {
        for (auto &child : _children)
            child->paint(ctx);
    }

    void print(Print::Context &ctx) override {
        for (auto &child : _children)
            child->print(ctx);
    }

    void repr(Io::Emit &e) const override {
        e("(stack");
        for (auto &child : _children) {
            e(" ");
            child->repr(e);
        }
        e(")");
    }
};

} // namespace Vaev::Paint
