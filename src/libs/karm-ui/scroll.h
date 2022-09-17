#pragma once

#include "funcs.h"
#include "proxy.h"

namespace Karm::Ui {

struct Scroll : public Proxy<Scroll> {
    Math::Vec2i _size{};
    Math::Recti _bound{};
    Math::Vec2i _scroll{};

    Scroll(Math::Vec2i size, Child child)
        : Proxy(child), _size(size) {}

    void paint(Gfx::Context &g, Math::Recti r) override {
        g.save();
        g.clip(_bound);
        g.origin(_scroll);
        r.xy = r.xy - _scroll;
        child().paint(g, r);
        g.restore();
    }

    void event(Events::Event &e) override {
        if (e.is<Events::MouseEvent>()) {
            auto ee = e.unwrap<Events::MouseEvent>();
            if (bound().contains(ee.pos)) {
                ee.pos = ee.pos - _scroll;
                child().event(ee);

                if (!ee.accepted) {
                    if (ee.type == Events::MouseEvent::SCROLL) {
                        auto childBound = child().bound();
                        _scroll = _scroll + ee.scroll * 8;

                        _scroll.x = clamp(_scroll.x, -(childBound.width - min(childBound.width, bound().width)), 0);
                        _scroll.y = clamp(_scroll.y, -(childBound.height - min(childBound.height, bound().height)), 0);

                        shouldAnimate(*this);
                    }
                }
            }
        } else if (e.is<Events::AnimateEvent>()) {
            shouldRepaint(*parent(), bound());
        } else {
            child().event(e);
        }
    }

    void bubble(Events::Event &e) override {
        if (e.is<Events::PaintEvent>()) {
            auto &paintEvent = e.unwrap<Events::PaintEvent>();
            paintEvent.bound.xy = paintEvent.bound.xy + _scroll;
            paintEvent.bound = paintEvent.bound.clipTo(bound());
        }

        Proxy::bubble(e);
    }

    void layout(Math::Recti r) override {
        _bound = r;
        auto childSize = child().size(_bound.size(), Layout::Hint::MAX);
        r.wh = childSize;
        child().layout(r);
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        if (hint == Layout::Hint::MIN)
            return _size;
        else
            return s;
    }

    Math::Recti bound() override {
        return _bound;
    }
};

static inline Child scroll(Math::Vec2i size, Child child) {
    return makeStrong<Scroll>(size, child);
}

static inline Child scroll(Child child) {
    return makeStrong<Scroll>(Math::Vec2i{}, child);
}

} // namespace Karm::Ui
