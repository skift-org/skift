#pragma once

#include <karm-scene/base.h>
#include <karm-scene/stack.h>
#include <karm-scene/viewbox.h>

#include "paper.h"
#include "printer.h"

namespace Karm::Print {

struct Page {
    PaperStock _paper;
    Rc<Scene::Node> _content;

    Page(PaperStock paper, Opt<Rc<Scene::Node>> content = NONE)
        : _paper(paper), _content(content ? content.take() : makeRc<Scene::Stack>()) {}

    Rc<Scene::Node> content() const {
        return makeRc<Scene::Viewbox>(_paper.size(), _content);
    }

    void print(Print::Printer& doc, Scene::PaintOptions o = {.showBackgroundGraphics = false}) {
        auto& canvas = doc.beginPage(_paper);
        content()->paint(canvas, _paper.size().cast<f64>(), o);
    }

    void repr(Io::Emit& e) const {
        e("(page paper:{} root:{})", _paper, content());
    }
};

} // namespace Karm::Print
