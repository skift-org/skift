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

    Scroll(Child child, Layout::Orien orient)
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

    void event(Async::Event &e) override {
        if (auto *me = e.is<Events::MouseEvent>()) {
            if (bound().contains(me->pos)) {
                _mouseIn = true;

                me->pos = me->pos - _scroll.cast<isize>();
                child().event(e);
                me->pos = me->pos + _scroll.cast<isize>();

                if (not e.accepted()) {
                    if (me->type == Events::MouseEvent::SCROLL) {
                        scroll((_scroll + me->scroll * 128).cast<isize>());
                        shouldAnimate(*this);
                        _animated = true;
                    }
                }
            } else if (_mouseIn) {
                _mouseIn = false;
                mouseLeave(*_child);
            }
        } else if (e.is<Node::AnimateEvent>() and _animated) {
            shouldRepaint(*parent(), bound());

            _scroll = _scroll + (_targetScroll - _scroll) * (e.unwrap<Node::AnimateEvent>().dt * 20);

            if (_scroll.dist(_targetScroll) < 0.5) {
                _scroll = _targetScroll;
                _animated = false;
            } else {
                shouldAnimate(*this);
            }

        } else {
            ProxyNode<Scroll>::event(e);
        }
    }

    void bubble(Async::Event &e) override {
        if (auto *pe = e.is<Node::PaintEvent>()) {
            pe->bound.xy = pe->bound.xy + _scroll.cast<isize>();
            pe->bound = pe->bound.clipTo(bound());
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
        }

        return childSize;
    }

    Math::Recti bound() override {
        return _bound;
    }
};

Child vhscroll(Child child) {
    return makeStrong<Scroll>(child, Layout::Orien::BOTH);
}

Child hscroll(Child child) {
    return makeStrong<Scroll>(child, Layout::Orien::HORIZONTAL);
}

Child vscroll(Child child) {
    return makeStrong<Scroll>(child, Layout::Orien::VERTICAL);
}

} // namespace Karm::Ui
