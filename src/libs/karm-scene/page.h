#pragma once

#include "stack.h"

namespace Karm::Scene {

struct Page : public Stack {
    Math::Vec2i _size;

    Page(Math::Vec2i size) : _size(size) {}

    void print(Print::Printer &doc) override {
        Stack::print(doc);
        paint(doc.beginPage(), _size.cast<f64>());
    }

    Math::Rectf bound() override {
        return _size.cast<f64>();
    }

    void repr(Io::Emit &e) const override {
        e("(page");
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
