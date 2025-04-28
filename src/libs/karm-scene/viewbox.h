
#include "base.h"

namespace Karm::Scene {

struct Viewbox : Node {
    Rc<Node> _content;
    Math::Rectf _viewbox;

    Viewbox(Math::Rectf viewbox, Rc<Node> content)
        : _content(content), _viewbox(viewbox) {}

    void prepare() override {
        _content->prepare();
    }

    Math::Rectf bound() override {
        return _viewbox.size();
    }

    void paint(Gfx::Canvas& g, Math::Rectf r, PaintOptions o) override {
        if (not bound().colide(r))
            return;

        g.push();
        g.origin(_viewbox.xy);
        g.clip(_viewbox);

        _content->paint(g, r, o);

        g.pop();
    }

    void repr(Io::Emit& e) const override {
        e("(viewbox {} content:{})", _viewbox, _content);
    }
};

} // namespace Karm::Scene
