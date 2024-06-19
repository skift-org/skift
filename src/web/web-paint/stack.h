#pragma once

#include "base.h"

namespace Web::Paint {

struct Stack : public Node {
    Vec<Strong<Node>> _children;

    void prepare() override {
        stableSort(_children, [](auto &a, auto &b) {
            return a->attrs().zIndex < b->attrs().zIndex;
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
};

} // namespace Web::Paint
