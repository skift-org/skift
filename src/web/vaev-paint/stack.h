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

    void paint(Gfx::Canvas &g) override {
        for (auto &child : _children)
            child->paint(g);
    }

    void print(Print::Printer &p) override {
        for (auto &child : _children)
            child->print(p);
    }

    void repr(Io::Emit &e) const override {
        e("(stack");
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

} // namespace Vaev::Paint
