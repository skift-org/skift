#include "anim.h"

namespace Karm::Ui {

/* --- Slide In ------------------------------------------------------------- */

struct SlideIn : public ProxyNode<SlideIn> {
    SlideFrom _from;
    Easedf _slide{};

    SlideIn(SlideFrom from, Ui::Child child)
        : ProxyNode(std::move(child)),
          _from(from) {
    }

    Math::Vec2f outside() {
        switch (_from) {
        case SlideFrom::START:
            return {(f64)-bound().width, 0};

        case SlideFrom::END:
            return {(f64)bound().width, 0};

        case SlideFrom::TOP:
            return {0, (f64)-bound().height};

        case SlideFrom::BOTTOM:
            return {0, (f64)bound().height};
        }
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        g.save();

        g.clip(bound());
        auto anim = lerp(outside(), Math::Vec2f{}, _slide.value());
        g.origin(anim.cast<isize>());
        r.xy = r.xy - anim.cast<isize>();
        child().paint(g, r);

        g.restore();
    }

    void event(Async::Event &e) override {
        if (_slide.needRepaint(*this, e))
            Ui::shouldRepaint(*this, bound());

        Ui::ProxyNode<SlideIn>::event(e);
    }

    void attach(Node *parent) override {
        Ui::ProxyNode<SlideIn>::attach(parent);
        _slide.animate(*this, 1.0, 0.25, Math::Easing::cubicOut);
    }
};

Child slideIn(SlideFrom from, Ui::Child child) {
    return makeStrong<SlideIn>(from, std::move(child));
}

/* --- Slide In/Out --------------------------------------------------------- */

struct SlideInOut : public ProxyNode<SlideInOut> {
    enum State {
        IDLE,
        IN,
        OUT,
    };
    bool _visible{};
    SlideFrom _from;
    Easedf _slide{};

    State _state{};
    Math::Rectf _oldBound{};

    SlideInOut(bool visible, SlideFrom from, Ui::Child child)
        : ProxyNode(std::move(child)),
          _visible(visible),
          _from(from) {
    }

    void reconcile(SlideInOut &o) override {
        _visible = o._visible;
        _from = o._from;
        ProxyNode<SlideInOut>::reconcile(o);
    }
};

Child slideInOut(bool visible, SlideFrom from, Ui::Child child) {
    return makeStrong<SlideInOut>(visible, from, std::move(child));
}

} // namespace Karm::Ui
