module;

#include <karm-gfx/canvas.h>
#include <karm-math/rect.h>

export module Karm.Ui:popover;

import Karm.App;
import :dialog;
import :funcs;

namespace Karm::Ui {

export struct ShowPopoverEvent {
    Math::Vec2i at;
    Child child;
};

export struct ClosePopoverEvent {};

export void showPopover(Node& n, Math::Vec2i at, Child child) {
    bubble<ShowPopoverEvent>(n, at, child);
}

export void closePopover(Node& n) {
    bubble<ClosePopoverEvent>(n);
}

struct PopoverLayer : ProxyNode<PopoverLayer> {
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

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        ProxyNode::paint(g, r);

        if (_popover)
            (*_popover)->paint(g, r);
    }

    void event(App::Event& e) override {
        if (e.accepted())
            return;

        bool allowToGoDown = e.is<ShowDialogEvent>();

        if (_popover and not allowToGoDown) {
            (*_popover)->event(e);

            if (e.accepted())
                return;

            auto me = e.is<App::MouseEvent>();
            if (me and me->type == App::MouseEvent::PRESS) {
                _closePopover();
                e.accept();
            }
        } else {
            ProxyNode::event(e);
        }
    }

    void bubble(App::Event& event) override {
        if (auto e = event.is<ShowPopoverEvent>()) {
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

export Child popoverLayer(Child child) {
    return makeRc<PopoverLayer>(child);
}

export auto popoverLayer() {
    return [](Child child) {
        return popoverLayer(child);
    };
}

} // namespace Karm::Ui
