export module Karm.Print:page;

import Karm.Core;
import Karm.Scene;
import :paper;
import :printer;

namespace Karm::Print {

export struct Page {
    PaperStock _paper;
    Rc<Scene::Node> _content;

    Page(PaperStock paper, Opt<Rc<Scene::Node>> content = NONE)
        : _paper(paper), _content(content ? content.take() : makeRc<Scene::Stack>()) {}

    Rc<Scene::Node> content() const {
        return makeRc<Scene::Viewbox>(_content, _paper.size());
    }

    void print(Printer& doc, Scene::PaintOptions o = {.showBackgroundGraphics = false}) {
        auto& canvas = doc.beginPage(_paper);
        content()->paint(canvas, _paper.size().cast<f64>(), o);
    }

    void repr(Io::Emit& e) const {
        e("(page paper:{} root:{})", _paper, content());
    }
};

} // namespace Karm::Print
