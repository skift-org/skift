#include "anim.h"

namespace Karm::Ui {

// MARK: Slide In --------------------------------------------------------------

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

    auto translation() {
        return lerp(outside(), Math::Vec2f{}, _slide.value()).cast<isize>();
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        g.save();

        g.clip(bound());
        auto anim = translation();
        g.origin(anim);
        r.xy = r.xy - anim;
        child().paint(g, r);

        g.restore();
    }

    void event(Sys::Event &e) override {
        if (_slide.needRepaint(*this, e)) {
            auto repaintBound =
                bound().clipTo(
                    child().bound().offset(translation())
                );

            Ui::shouldRepaint(*this, repaintBound);
        }

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

// MARK: Carousel --------------------------------------------------------------

struct Carousel : public GroupNode<Carousel> {
    usize _selected;
    Layout::Flow _flow;
    Easedf _slide{};

    Carousel(usize selected, Children children, Layout::Flow flow)
        : GroupNode(children), _selected(selected), _flow(flow) {
    }

    void reconcile(Carousel &o) override {
        GroupNode::reconcile(o);
        if (_selected != o._selected) {
            _selected = o._selected;
            _slide.animate(*this, _selected, 0.3, Math::Easing::cubicOut);
        }
    }

    Math::Vec2i translation() {
        return {
            (int)(-_slide.value() * bound().width),
            0,
        };
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        g.save();
        g.clip(bound());
        auto anim = translation();
        g.origin(anim);
        for (auto &child : children()) {
            child->paint(g, r);
        }
        g.restore();
    }

    void event(Sys::Event &e) override {
        if (_slide.needRepaint(*this, e)) {
            Ui::shouldRepaint(*this, bound());
        }

        GroupNode::event(e);
    }

    void layout(Math::Recti r) override {
        _bound = r;
        for (auto &child : children()) {
            child->layout(r);
            r = r.offset({r.width, 0});
        }
    }
};

Child carousel(usize selected, Children children, Layout::Flow flow) {
    return makeStrong<Carousel>(selected, std::move(children), flow);
}

} // namespace Karm::Ui
