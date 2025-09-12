module;

#include <karm-gfx/canvas.h>

export module Karm.Kira:progress;

import Karm.App;
import Karm.Ui;

namespace Karm::Kira {

struct Progress : Ui::View<Progress> {
    isize _size;
    f64 _spin = 0;

    Progress(isize size)
        : _size(size) {
    }

    void reconcile(Progress& o) override {
        _size = o._size;
    }

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();
        g.clip(bound());

        g.translate(bound().center().cast<f64>());
        g.rotate(_spin);

        g.beginPath();
        g.arc({
            {0, 0},
            min(bound().width / 2., bound().height / 2.),
            0,
            Math::PI * 1.25,
        });

        g.stroke(
            Gfx::stroke(Ui::ACCENT500)
                .withWidth(bound().width / 8.)
                .withAlign(Gfx::INSIDE_ALIGN)
                .withCap(Gfx::ROUND_CAP)
        );

        g.pop();
    }

    void event(App::Event& e) override {
        if (auto ae = e.is<Node::AnimateEvent>()) {
            _spin += ae->dt * 8;
            Ui::shouldAnimate(*this);
            Ui::shouldRepaint(*this);
        }

        Ui::View<Progress>::event(e);
    }

    Math::Vec2i size(Math::Vec2i, Ui::Hint) override {
        return _size;
    }
};

export Ui::Child progress(isize size = 16) {
    return makeRc<Progress>(size);
}

} // namespace Karm::Kira
