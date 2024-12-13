#pragma once

#include "stack.h"

namespace Karm::Scene {

struct Page : public Stack {
    Print::PaperStock _paper;

    Page(Print::PaperStock paper, Opt<Math::Trans2f> transform = NONE)
        : Stack(transform), _paper(paper) {}

    void print(Print::Printer &doc, PaintOptions o) override {
        Stack::print(doc, o);
        paint(doc.beginPage(_paper), _paper.size().cast<f64>(), o);
    }

    Math::Rectf bound() override {
        return _paper.size().cast<f64>();
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
