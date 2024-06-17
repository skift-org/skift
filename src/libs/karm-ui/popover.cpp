#include "popover.h"

#include "funcs.h"

namespace Karm::Ui {

struct ShowPopoverEvent {
    Math::Vec2i at;
    Child child;
};

struct ClosePopoverEvent {};

void showPopover(Node &n, Math::Vec2i at, Child child) {
    bubble<ShowPopoverEvent>(n, at, child);
}

void closePopover(Node &n) {
    bubble<ClosePopoverEvent>(n);
}

struct PopoverLayer : public LeafNode<PopoverLayer> {
    Child _child;

    Opt<Child> _popover;
    Opt<Child> _shouldPopover;
    bool _shouldPopoverClose = false;
    Math::Vec2i _popoverAt;

    PopoverLayer(Child child) : _child(child) {
        _child->attach(this);
    }

    ~PopoverLayer() {
        if (_popover) {
            (*_popover)->detach(this);
        }

        _child->detach(this);
    }

    Node &child() {
        return *_child;
    }

    Node const &child() const {
        return *_child;
    }

    Node &popover() { return **_popover; }

    Node const &popover() const { return **_popover; }

    bool popoverVisible() const {
        return (bool)_popover;
    }

    void reconcile(PopoverLayer &o) override {
        _child = tryOr(_child->reconcile(o._child), _child);
        _child->attach(this);
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        child().paint(g, r);

        if (popoverVisible()) {
            (*_popover)->paint(g, r);
        }
    }

    void event(Sys::Event &e) override {
        if (popoverVisible()) {
            popover().event(e);
            e.handle<Events::MouseEvent>([&](auto &m) {
                if (m.type == Events::MouseEvent::PRESS) {
                    _shouldPopoverClose = true;
                    shouldLayout(*this);
                    return true;
                }
                return false;
            });
        }
        child().event(e);
    }

    void bubble(Sys::Event &e) override {
        if (e.is<ShowPopoverEvent>()) {
            // We need to defer showing the popover until the next frame,
            // otherwise replacing the popover might cause some use after free down the tree
            auto &s = e.unwrap<ShowPopoverEvent>();
            _shouldPopover = s.child;
            _popoverAt = s.at;
            mouseLeave(*_child);
            shouldLayout(*this);
        } else if (e.is<ClosePopoverEvent>()) {
            // We need to defer closing the dialog until the next frame,
            // otherwise we might cause some use after free down the tree
            _shouldPopoverClose = true;
            shouldLayout(*this);
        } else if (parent()) {
            parent()->bubble(e);
        }
    }

    void layout(Math::Recti r) override {
        if (_shouldPopoverClose) {
            if (_popover) {
                (*_popover)->detach(this);
                _popover = NONE;
            }
            _shouldPopoverClose = false;
        }

        if (_shouldPopover) {
            if (_popover) {
                (*_popover)->detach(this);
            }
            _popover = _shouldPopover;
            (*_popover)->attach(this);
            _shouldPopover = NONE;
        }

        child().layout(r);

        if (popoverVisible()) {
            auto size = (*_popover)->size(r.size(), Hint::MIN);
            auto pos = _popoverAt;
            pos.y = clamp(pos.y, 0, r.size().y - size.y);

            if (pos.x + size.x > r.end()) {
                pos.x = pos.x - size.x;
            }

            (*_popover)->layout({pos, size});
        }
    }

    Math::Vec2i size(Math::Vec2i s, Hint hint) override {
        return child().size(s, hint);
    }

    Math::Recti bound() override {
        return _child->bound();
    }
};

Child popoverLayer(Child child) {
    return makeStrong<PopoverLayer>(child);
}

} // namespace Karm::Ui
