#include "scroll.h"

#include "anim.h"

namespace Karm::Ui {

// MARK: Scroll ----------------------------------------------------------------

struct Scroll : public ProxyNode<Scroll> {
    bool _mouseIn = false;
    Math::Orien _orient{};
    Math::Recti _bound{};
    Math::Vec2f _scroll{};
    Math::Vec2f _targetScroll{};

    // For momentum scrolling
    Easedf _scrollOpacity;

    bool _animated = false;
    Math::Vec2f _acceleration; // Scroll acceleration
    Math::Vec2f _momentum;     // Scroll velocity

    static constexpr f64 DECELERATION = 0.85; // Factor for deceleration
    static constexpr f64 RESISTANCE = 0.85;   // Resistance to bring scroll back in bounds

    Scroll(Child child, Math::Orien orient)
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
            g.plot(child().bound(), Gfx::PINK);

        g.restore();

        if (debugShowScrollBounds)
            g.plot(_bound, Gfx::CYAN);

        // draw scroll bar
        g.save();
        g.clip(_bound);

        auto childBound = child().bound();

        if ((_orient == Math::Orien::HORIZONTAL or _orient == Math::Orien::BOTH) and childBound.width > bound().width) {
            auto scrollBarWidth = (bound().width) * bound().width / childBound.width;
            auto scrollBarX = bound().start() + (-_scroll.x * bound().width / childBound.width);

            g.fillStyle(Gfx::GRAY500.withOpacity(0.3 * _scrollOpacity.value()));
            g.fill(Math::Recti{(isize)scrollBarX, bound().bottom() - 4, scrollBarWidth, 4});
        }

        if ((_orient == Math::Orien::VERTICAL or _orient == Math::Orien::BOTH) and childBound.height > bound().height) {
            auto scrollBarHeight = (bound().height) * bound().height / childBound.height;
            auto scrollBarY = bound().top() + (-_scroll.y * bound().height / childBound.height);

            g.fillStyle(Ui::GRAY500.withOpacity(0.3 * _scrollOpacity.value()));
            g.fill(Math::Recti{bound().end() - 4, (isize)scrollBarY, 4, scrollBarHeight});
        }

        g.restore();
    }

    void event(Sys::Event &e) override {
        _scrollOpacity.update(*this, e);

        if (auto *me = e.is<Events::MouseEvent>()) {
            if (bound().contains(me->pos)) {
                _mouseIn = true;

                me->pos = me->pos - _scroll.cast<isize>();
                ProxyNode<Scroll>::event(e);
                me->pos = me->pos + _scroll.cast<isize>();

                if (not e.accepted()) {
                    if (me->type == Events::MouseEvent::SCROLL) {
                        _acceleration = _acceleration + me->scroll / (1.0 / 200.0);
                        _animated = true;
                        shouldAnimate(*this); // Start animation to apply momentum
                        _scrollOpacity.animate(*this, 1, 0.3);
                    }
                }
            } else if (_mouseIn) {
                _mouseIn = false;
                mouseLeave(*_child);
            }
        } else if (e.is<Node::AnimateEvent>()) {
            // Cancel momentum & acceleration
            // if we can't scroll in that direction
            if (_orient == Math::Orien::VERTICAL) {
                _momentum.x = 0;
                _acceleration.x = 0;
            }

            if (_orient == Math::Orien::HORIZONTAL) {
                _momentum.y = 0;
                _acceleration.y = 0;
            }

            // Cancel momentum if child is smaller than scroll area
            auto bound = this->bound();
            auto childBound = child().bound();

            if (childBound.width <= bound.width) {
                _momentum.x = 0;
                _acceleration.x = 0;
            }

            if (childBound.height <= bound.height) {
                _momentum.y = 0;
                _acceleration.y = 0;
            }

            // Is the scroll position out of bounds?
            // Apply a force to bring it back in bounds
            auto overflow = childBound.size() - min(childBound.size(), bound.size());

            if (_scroll.x > 0) {
                _acceleration.x = _acceleration.x - _scroll.x * RESISTANCE;
            } else if (_scroll.x < -overflow.x) {
                _acceleration.x = _acceleration.x - (_scroll.x + overflow.x) * RESISTANCE;
            }

            if (_scroll.y > 0) {
                _acceleration.y = _acceleration.y - _scroll.y * RESISTANCE;
            } else if (_scroll.y < -overflow.y) {
                _acceleration.y = _acceleration.y - (_scroll.y + overflow.y) * RESISTANCE;
            }

            // Apply deceleration
            _momentum = _momentum * DECELERATION;

            // Apply acceleration
            _momentum = _momentum + _acceleration;
            _acceleration = 0;

            // Apply momentum to scroll position
            if (Math::epsilonEq(_momentum.x, 0.0, 1.0) and
                Math::epsilonEq(_momentum.y, 0.0, 1.0) and _animated) {
                _momentum = 0;
                _scrollOpacity.animate(*this, 0, 0.3);
                _animated = false;
            } else {
                _scroll = _scroll + _momentum * e.unwrap<Node::AnimateEvent>().dt;
                // NOTE: We have to tell our parent to repaint us instead of
                //       just calling shouldRepaint() because
                shouldRepaint(*parent(), bound);
                shouldAnimate(*this);
            }

            ProxyNode<Scroll>::event(e);
        } else {
            ProxyNode<Scroll>::event(e);
        }
    }

    void bubble(Sys::Event &e) override {
        if (auto *pe = e.is<Node::PaintEvent>()) {
            pe->bound.xy = pe->bound.xy + _scroll.cast<isize>();
            pe->bound = pe->bound.clipTo(bound());
        }

        ProxyNode::bubble(e);
    }

    void layout(Math::Recti r) override {
        _bound = r;
        auto childSize = child().size(_bound.size(), Hint::MAX);
        if (_orient == Math::Orien::HORIZONTAL) {
            childSize.height = r.height;
        } else if (_orient == Math::Orien::VERTICAL) {
            childSize.width = r.width;
        }
        r.wh = childSize;
        child().layout(r);
        scroll(_scroll.cast<isize>());
    }

    Math::Vec2i size(Math::Vec2i s, Hint hint) override {
        auto childSize = child().size(s, hint);

        if (hint == Hint::MIN) {
            if (_orient == Math::Orien::HORIZONTAL) {
                childSize.x = min(childSize.x, s.x);
            } else if (_orient == Math::Orien::VERTICAL) {
                childSize.y = min(childSize.y, s.y);
            } else {
                childSize = childSize.min(s);
            }
        }

        return childSize;
    }

    Math::Recti bound() override {
        return _bound;
    }
};

Child vhscroll(Child child) {
    return makeStrong<Scroll>(child, Math::Orien::BOTH);
}

Child hscroll(Child child) {
    return makeStrong<Scroll>(child, Math::Orien::HORIZONTAL);
}

Child vscroll(Child child) {
    return makeStrong<Scroll>(child, Math::Orien::VERTICAL);
}

} // namespace Karm::Ui
