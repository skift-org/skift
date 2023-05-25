#include "scroll.h"

namespace Karm::Ui {

/* --- Scroll --------------------------------------------------------------- */

struct Scroll : public ProxyNode<Scroll> {
    bool _mouseIn = false;
    bool _animated = false;
    Layout::Orien _orient{};
    Math::Recti _bound{};
    Math::Vec2f _scroll{};
    Math::Vec2f _targetScroll{};

    Scroll(Layout::Orien orient, Child child)
        : ProxyNode(child), _orient(orient) {}

    void scroll(Math::Vec2i s) {
        auto childBound = child().bound();
        _targetScroll.x = clamp(s.x, -(childBound.width - min(childBound.width, bound().width)), 0);
        _targetScroll.y = clamp(s.y, -(childBound.height - min(childBound.height, bound().height)), 0);
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        g.save();
        g.clip(_bound);
        g.origin(_scroll.cast<isize>());
        r.xy = r.xy - _scroll.cast<isize>();
        child().paint(g, r);

        if (debugShowScrollBounds)
            g.debugRect(child().bound(), Gfx::PINK);

        g.restore();

        if (debugShowScrollBounds)
            g.debugRect(_bound, Gfx::CYAN);
    }

    void event(Events::Event &e) override {
        if (e.is<Events::MouseEvent>()) {
            auto ee = e.unwrap<Events::MouseEvent>();
            if (bound().contains(ee.pos)) {
                _mouseIn = true;

                ee.pos = ee.pos - _scroll.cast<isize>();
                child().event(ee);

                if (not ee.accepted) {
                    if (ee.type == Events::MouseEvent::SCROLL) {
                        scroll((_scroll + ee.scrollPrecise * 128).cast<isize>());
                        shouldAnimate(*this);
                        _animated = true;
                    }
                }
            } else if (_mouseIn) {
                mouseLeave(*_child);
            }
            e.accepted = ee.accepted;
        } else if (e.is<Events::AnimateEvent>() and _animated) {
            shouldRepaint(*parent(), bound());

            // Same as _scroll = _scroll + (_targetScroll - _scroll) * 0.2; but frame rate independent
            _scroll = _scroll + (_targetScroll - _scroll) * (e.unwrap<Events::AnimateEvent>().dt * 20);

            if (_scroll.dist(_targetScroll) < 0.5) {
                _scroll = _targetScroll;
                _animated = false;
            } else
                shouldAnimate(*this);

        } else {
            child().event(e);
        }
    }

    void bubble(Events::Event &e) override {
        if (e.is<Events::PaintEvent>()) {
            auto &paintEvent = e.unwrap<Events::PaintEvent>();
            paintEvent.bound.xy = paintEvent.bound.xy + _scroll.cast<isize>();
            paintEvent.bound = paintEvent.bound.clipTo(bound());
        }

        ProxyNode::bubble(e);
    }

    void layout(Math::Recti r) override {
        _bound = r;
        auto childSize = child().size(_bound.size(), Layout::Hint::MAX);
        if (_orient == Layout::Orien::HORIZONTAL) {
            childSize.height = r.height;
        } else if (_orient == Layout::Orien::VERTICAL) {
            childSize.width = r.width;
        }
        r.wh = childSize;
        child().layout(r);
        scroll(_scroll.cast<isize>());
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        auto childSize = child().size(s, hint);

        if (hint == Layout::Hint::MIN) {
            if (_orient == Layout::Orien::HORIZONTAL) {
                childSize.x = s.x;
            } else if (_orient == Layout::Orien::VERTICAL) {
                childSize.y = s.y;
            } else {
                childSize = s;
            }
            return childSize;
        } else {
            return childSize;
        }
    }

    Math::Recti bound() override {
        return _bound;
    }
};

Child vhscroll(Child child) {
    return makeStrong<Scroll>(Layout::Orien::BOTH, child);
}

Child hscroll(Child child) {
    return makeStrong<Scroll>(Layout::Orien::HORIZONTAL, child);
}

Child vscroll(Child child) {
    return makeStrong<Scroll>(Layout::Orien::VERTICAL, child);
}

/* --- List ----------------------------------------------------------------- */

struct List : public GroupNode<List> {
    usize _count;
    BuildItem _builder;

    List(usize count, BuildItem builder)
        : _count(count),
          _builder(std::move(builder)) {}
};

} // namespace Karm::Ui
