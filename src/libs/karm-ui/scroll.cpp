module;

#include <karm-gfx/canvas.h>
#include <karm-math/flow.h>

export module Karm.Ui:scroll;

import Karm.App;
import :anim;
import :node;
import :atoms;

namespace Karm::Ui {

// MARK: Scroll ----------------------------------------------------------------

struct Scroll : ProxyNode<Scroll> {
    static constexpr isize SCROLL_BAR_WIDTH = 4;

    bool _mouseIn = false;
    bool _animated = false;
    Math::Orien _orient{};
    Math::Recti _bound{};
    Math::Vec2f _scroll{};
    Math::Vec2f _targetScroll{};
    Easedf _scrollOpacity;

    Scroll(Child child, Math::Orien orient)
        : ProxyNode(child), _orient(orient) {}

    void scroll(Math::Vec2i s) {
        auto childBound = child().bound();
        _targetScroll.x = clamp(s.x, -(childBound.width - min(childBound.width, bound().width)), 0);
        _targetScroll.y = clamp(s.y, -(childBound.height - min(childBound.height, bound().height)), 0);
        if (_scroll.dist(_targetScroll) < 0.5) {
            _scroll = _targetScroll;
            _animated = false;
        } else {
            _animated = true;
        }
    }

    bool canHScroll() {
        return (_orient == Math::Orien::HORIZONTAL or _orient == Math::Orien::BOTH) and child().bound().width > bound().width;
    }

    Math::Recti hTrack() {
        return Math::Recti{bound().start(), bound().bottom() - SCROLL_BAR_WIDTH, bound().width, SCROLL_BAR_WIDTH};
    }

    bool canVScroll() {
        return (_orient == Math::Orien::VERTICAL or _orient == Math::Orien::BOTH) and child().bound().height > bound().height;
    }

    Math::Recti vTrack() {
        return Math::Recti{bound().end() - SCROLL_BAR_WIDTH, bound().top(), SCROLL_BAR_WIDTH, bound().height};
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        g.push();
        g.clip(_bound);
        g.origin(_scroll);
        r.xy = r.xy - _scroll.cast<isize>();
        child().paint(g, r);

        g.pop();

        // draw scroll bar
        g.push();
        g.clip(_bound);

        auto childBound = child().bound();

        if (canHScroll()) {
            auto scrollBarWidth = (bound().width) * bound().width / childBound.width;
            auto scrollBarX = bound().start() + (-_scroll.x * bound().width / childBound.width);

            g.fillStyle(Gfx::GRAY500.withOpacity(0.5 * clamp01(_scrollOpacity.value())));
            g.fill(Math::Recti{(isize)scrollBarX, bound().bottom() - SCROLL_BAR_WIDTH, scrollBarWidth, SCROLL_BAR_WIDTH});
        }

        if (canVScroll()) {
            auto scrollBarHeight = (bound().height) * bound().height / childBound.height;
            auto scrollBarY = bound().top() + (-_scroll.y * bound().height / childBound.height);

            g.fillStyle(Ui::GRAY500.withOpacity(0.5 * clamp01(_scrollOpacity.value())));
            g.fill(Math::Recti{bound().end() - SCROLL_BAR_WIDTH, (isize)scrollBarY, SCROLL_BAR_WIDTH, scrollBarHeight});
        }

        g.pop();
    }

    void event(App::Event& e) override {
        if (_scrollOpacity.needRepaint(*this, e)) {
            if (canHScroll())
                shouldRepaint(*parent(), hTrack());

            if (canVScroll())
                shouldRepaint(*parent(), vTrack());
        }

        if (auto me = e.is<App::MouseEvent>()) {
            if (bound().contains(me->pos)) {
                _mouseIn = true;

                me->pos = me->pos - _scroll.cast<isize>();
                ProxyNode::event(e);
                me->pos = me->pos + _scroll.cast<isize>();

                if (not e.accepted()) {
                    if (me->type == App::MouseEvent::SCROLL) {
                        if (_orient == Math::Orien::BOTH) {
                            scroll((_scroll + me->scroll * 128).cast<isize>());
                        } else if (_orient == Math::Orien::HORIZONTAL) {
                            scroll((_scroll + Math::Vec2f{(me->scroll.x + me->scroll.y) * 128, 0}).cast<isize>());
                        } else if (_orient == Math::Orien::VERTICAL) {
                            scroll((_scroll + Math::Vec2f{0, (me->scroll.x + me->scroll.y) * 128}).cast<isize>());
                        }
                        shouldAnimate(*this);
                        _scrollOpacity.delay(0).animate(*this, 1, 0.3);
                        e.accept();
                    }
                }
            } else if (_mouseIn) {
                _mouseIn = false;
                mouseLeave(*_child);
            }
        } else if (e.is<Node::AnimateEvent>() and _animated) {
            shouldRepaint(*parent(), bound());

            auto delta = _targetScroll - _scroll;

            _scroll = _scroll + delta * (e.unwrap<Node::AnimateEvent>().dt * 12);

            if (_scroll.dist(_targetScroll) < 0.5) {
                _scroll = _targetScroll;
                _animated = false;
                _scrollOpacity.delay(1.0).animate(*this, 0, 0.3);
            } else {
                shouldAnimate(*this);
            }
            ProxyNode<Scroll>::event(e);
        } else {
            ProxyNode<Scroll>::event(e);
        }
    }

    void bubble(App::Event& e) override {
        if (auto pe = e.is<Node::PaintEvent>()) {
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

        // Make sure the child is at least as big as the parent
        childSize.width = max(childSize.width, r.width);
        childSize.height = max(childSize.height, r.height);

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

export Child vhscroll(Child child) {
    return makeRc<Scroll>(child, Math::Orien::BOTH);
}

export auto vhscroll() {
    return [](Child child) {
        return vhscroll(child);
    };
}

export Child hscroll(Child child) {
    return makeRc<Scroll>(child, Math::Orien::HORIZONTAL);
}

export auto hscroll() {
    return [](Child child) {
        return hscroll(child);
    };
}

export Child vscroll(Child child) {
    return makeRc<Scroll>(child, Math::Orien::VERTICAL);
}

export auto vscroll() {
    return [](Child child) {
        return vscroll(child);
    };
}

// MARK: Clip ------------------------------------------------------------------

struct Clip : ProxyNode<Clip> {
    static constexpr isize SCROLL_BAR_WIDTH = 4;

    Math::Orien _orient{};
    Math::Recti _bound{};

    Clip(Child child, Math::Orien orient)
        : ProxyNode(child), _orient(orient) {}

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        g.push();
        g.clip(_bound);
        child().paint(g, r);
        g.pop();
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

export Child vhclip(Child child) {
    return makeRc<Clip>(child, Math::Orien::BOTH);
}

export auto vhclip() {
    return [](Child child) {
        return vhclip(child);
    };
}

export Child hclip(Child child) {
    return makeRc<Clip>(child, Math::Orien::HORIZONTAL);
}

export auto hclip() {
    return [](Child child) {
        return hclip(child);
    };
}

export Child vclip(Child child) {
    return makeRc<Clip>(child, Math::Orien::VERTICAL);
}

export auto vclip() {
    return [](Child child) {
        return vclip(child);
    };
}

} // namespace Karm::Ui
