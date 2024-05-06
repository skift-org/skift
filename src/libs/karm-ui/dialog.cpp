#include "dialog.h"

#include "anim.h"
#include "funcs.h"

namespace Karm::Ui {

struct ShowDialogEvent {
    Child child;
};

void showDialog(Node &n, Child child) {
    bubble<ShowDialogEvent>(n, child);
}

struct CloseDialogEvent {
};

void closeDialog(Node &n) {
    bubble<CloseDialogEvent>(n);
}

struct ShowPopoverEvent {
    Math::Vec2i at;
    Child child;
};

void showPopover(Node &n, Math::Vec2i at, Child child) {
    bubble<ShowPopoverEvent>(n, at, child);
}

void closePopover(Node &n) {
    struct ClosePopoverEvent {};
    bubble<ClosePopoverEvent>(n);
}

struct DialogLayer : public LeafNode<DialogLayer> {
    Easedf _visibility{};
    Child _child;

    Opt<Child> _dialog;
    Opt<Child> _shouldShow;
    bool _shouldDialogClose = false;

    Opt<Child> _popover;
    Opt<Child> _shouldPopover;
    bool _shouldPopoverClose = false;
    Math::Vec2i _popoverAt;

    DialogLayer(Child child) : _child(child) {
        _child->attach(this);
    }

    ~DialogLayer() {
        if (_dialog) {
            (*_dialog)->detach(this);
        }

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

    Node &dialog() { return **_dialog; }

    Node const &dialog() const { return **_dialog; }

    Node &popover() { return **_popover; }

    Node const &popover() const { return **_popover; }

    bool dialogVisible() const {
        return (bool)_dialog;
    }

    bool popoverVisible() const {
        return (bool)_popover;
    }

    void reconcile(DialogLayer &o) override {
        _child = tryOr(_child->reconcile(o._child), _child);
        _child->attach(this);
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        child().paint(g, r);

        if (_visibility.value() > 0.001) {
            g.save();
            g.fillStyle(Ui::GRAY950.withOpacity(0.8 * _visibility.value()));
            g.fill(bound());
            g.restore();
        }

        if (dialogVisible()) {

            g.save();
            // change the orgin to the center of the screen
            g.translate(bound().center().cast<f64>());
            g.scale(Math::lerp(0.9, 1, _visibility.value()));
            g.translate(-bound().center().cast<f64>());

            dialog().paint(g, r);

            g.restore();
        }
    }

    void event(Sys::Event &e) override {
        if (_visibility.needRepaint(*this, e))
            Ui::shouldRepaint(*this);

        if (popoverVisible()) {
            popover().event(e);
        } else if (dialogVisible()) {
            dialog().event(e);
        } else {
            child().event(e);
        }
    }

    void bubble(Sys::Event &e) override {
        if (e.is<ShowDialogEvent>()) {
            // We need to defer showing the dialog until the next frame,
            // otherwise replacing the dialog might cause some use after free down the tree
            auto &s = e.unwrap<ShowDialogEvent>();
            _shouldShow = s.child;
            mouseLeave(*_child);
            shouldLayout(*this);
            _visibility.animate(*this, 1, 0.3, Math::Easing::exponentialOut);
        } else if (e.is<ShowPopoverEvent>()) {
            // We need to defer showing the popover until the next frame,
            // otherwise replacing the popover might cause some use after free down the tree
            auto &s = e.unwrap<ShowPopoverEvent>();
            _shouldPopover = s.child;
            _popoverAt = s.at;
            mouseLeave(*_child);
            shouldLayout(*this);
        } else if (e.is<CloseDialogEvent>()) {
            // We need to defer closing the dialog until the next frame,
            // otherwise we might cause some use after free down the tree
            _shouldDialogClose = true;
            shouldLayout(*this);
            _visibility.animate(*this, 0, 0.1);
        } else if (parent()) {
            parent()->bubble(e);
        }
    }

    void layout(Math::Recti r) override {
        if (_shouldDialogClose) {
            if (_dialog) {
                (*_dialog)->detach(this);
                _dialog = NONE;
            }
            _shouldDialogClose = false;
        }

        if (_shouldPopoverClose) {
            if (_popover) {
                (*_popover)->detach(this);
                _popover = NONE;
            }
            _shouldPopoverClose = false;
        }

        if (_shouldShow) {
            if (_dialog) {
                (*_dialog)->detach(this);
            }
            _dialog = _shouldShow;
            (*_dialog)->attach(this);
            _shouldShow = NONE;
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

        if (dialogVisible()) {
            (*_dialog)->layout(r);
        }

        if (popoverVisible()) {
            auto popoverSize = (*_popover)->size(r.size(), Hint::MIN);
            (*_popover)->layout({_popoverAt, popoverSize});
        }
    }

    Math::Vec2i size(Math::Vec2i s, Hint hint) override {
        return child().size(s, hint);
    }

    Math::Recti bound() override {
        return _child->bound();
    }
};

Child dialogLayer(Child child) {
    return makeStrong<DialogLayer>(child);
}

} // namespace Karm::Ui
