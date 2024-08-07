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

struct PopoverLayer : public ProxyNode<PopoverLayer> {
    Opt<Child> _popover;
    Opt<Child> _shouldPopover;
    bool _shouldPopoverClose = false;
    Math::Vec2i _popoverAt;

    using ProxyNode<PopoverLayer>::ProxyNode;

    ~PopoverLayer() {
        if (_popover)
            (*_popover)->detach(this);
    }

    void _showPopover(Child child, Math::Vec2i at) {
        // We need to defer showing the dialog until the next frame,
        // otherwise replacing the dialog might cause some use after free down the tree
        _shouldPopover = child;
        _popoverAt = at;
        shouldLayout(*this);
    }

    void _closePopover() {
        // We need to defer closing the dialog until the next frame,
        // otherwise we might cause some use after free down the tree
        _shouldPopoverClose = true;
        shouldLayout(*this);
    }

    Math::Recti _positionPopover(Math::Recti r) {
        // Position the popover at the given point, but make sure it fits in the screen
        auto size = (*_popover)->size(r.size(), Hint::MIN);
        auto pos = _popoverAt;
        pos.y = clamp(pos.y, 0, r.size().y - size.y);
        if (pos.x + size.x > r.end())
            pos.x = pos.x - size.x;
        return {pos, size};
    }

    void paint(Gfx::Canvas &g, Math::Recti r) override {
        ProxyNode::paint(g, r);

        if (_popover)
            (*_popover)->paint(g, r);
    }

    void event(App::Event &event) override {
        ProxyNode::event(event);

        if (event.accepted())
            return;

        if (not _popover)
            return;

        (*_popover)->event(event);

        if (event.accepted())
            return;

        auto *e = event.is<App::MouseEvent>();

        if (e and e->type == App::MouseEvent::PRESS) {
            _closePopover();
            event.accept();
        }
    }

    void bubble(App::Event &event) override {
        if (auto *e = event.is<ShowPopoverEvent>()) {
            _showPopover(e->child, e->at);
            event.accept();
        } else if (event.is<ClosePopoverEvent>()) {
            _closePopover();
            event.accept();
        }

        LeafNode<PopoverLayer>::bubble(event);
    }

    void layout(Math::Recti r) override {
        ProxyNode::layout(r);

        if (_shouldPopoverClose) {
            if (_popover) {
                (*_popover)->detach(this);
                _popover = NONE;
            }
            _shouldPopoverClose = false;
        }

        if (_shouldPopover) {
            if (_popover)
                (*_popover)->detach(this);

            _popover = _shouldPopover;
            (*_popover)->attach(this);
            _shouldPopover = NONE;
        }

        if (_popover)
            (*_popover)->layout(_positionPopover(r));
    }
};

Child popoverLayer(Child child) {
    return makeStrong<PopoverLayer>(child);
}

} // namespace Karm::Ui
