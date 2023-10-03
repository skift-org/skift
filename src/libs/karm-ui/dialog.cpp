#include "dialog.h"

#include "anim.h"
#include "box.h"
#include "drag.h"
#include "funcs.h"
#include "input.h"
#include "layout.h"
#include "scafold.h"
#include "view.h"

namespace Karm::Ui {

/* ---  Dialog Base  -------------------------------------------------------- */

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
    Easedf _opacity{};
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

        if (_opacity.value() > 0.001) {
            g.save();
            g.fillStyle(Gfx::BLACK.withOpacity(0.25 * _opacity.value()));
            g.fill(bound());
            g.restore();
        }

        if (dialogVisible()) {
            dialog().paint(g, r);
        }
    }

    void event(Async::Event &e) override {
        if (_opacity.needRepaint(*this, e)) {
            Ui::shouldRepaint(*this);
        }

        if (popoverVisible()) {
            popover().event(e);
        } else if (dialogVisible()) {
            dialog().event(e);
        } else {
            child().event(e);
        }
    }

    void bubble(Async::Event &e) override {
        if (e.is<ShowDialogEvent>()) {
            // We need to defer showing the dialog until the next frame,
            // otherwise replacing the dialog might cause some use after free down the tree
            auto &s = e.unwrap<ShowDialogEvent>();
            _shouldShow = s.child;
            mouseLeave(*_child);
            shouldLayout(*this);
            _opacity.animate(*this, 1, 0.1);
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
            _opacity.animate(*this, 0, 0.1);
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
            auto popoverSize = (*_popover)->size(r.size(), Layout::Hint::MIN);
            (*_popover)->layout({_popoverAt, popoverSize});
        }
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        return child().size(s, hint);
    }

    Math::Recti bound() override {
        return _child->bound();
    }
};

Child dialogLayer(Child child) {
    return makeStrong<DialogLayer>(child);
}

/* --- Dialogs Scaffolding -------------------------------------------------- */

Child dialogScafold(Layout::Align a, Child inner) {
    BoxStyle const boxStyle = {
        .borderRadius = 4,
        .borderWidth = 1,
        .borderPaint = GRAY800,
        .backgroundPaint = GRAY900,
    };

    return inner |
           box(boxStyle) |
           dragRegion() |
           align(a) |
           spacing(16);
}

Child dialogScafold(Layout::Align a, Child content, Children actions) {
    auto layout = minSize(
        {320, UNCONSTRAINED},
        spacing(
            16,
            vflow(
                32,
                grow(content),
                hflow(8, actions))));

    return dialogScafold(a, layout);
}

Child dialogCloseButton() {
    return button(
        closeDialog,
        ButtonStyle::primary(),
        "CLOSE");
}

/* --- Dialogs -------------------------------------------------------------- */

Child aboutDialog(Mdi::Icon i, String name) {
    auto content = vflow(
        8,
        Layout::Align::CENTER,
        spacing(16, icon(i, 48)),
        text(TextStyle::titleMedium(), name),
        empty(),
        badge(BadgeStyle::INFO, "v0.1.0"),
        empty(),
        text("Copyright © 2018-2023"),
        text("SMNX Research & contributors."));

    Children actions = {
        button(NONE, ButtonStyle::subtle(), "LICENSE"),
        grow(NONE),
        dialogCloseButton(),
    };

    return dialogScafold(
        Layout::Align::CENTER | Layout::Align::CLAMP,
        content,
        actions);
}

void showAboutDialog(Node &n, Mdi::Icon icon, String name) {
    showDialog(n, aboutDialog(icon, name));
}

Child msgDialog(String title, String msg) {
    auto titleLbl = text(TextStyle::titleMedium(), title);
    auto msgLbl = text(msg);
    Children actions = {
        grow(NONE),
        button(
            closeDialog,
            ButtonStyle::primary(), "OK"),
    };

    return dialogScafold(
        Layout::Align::CENTER,
        vflow(16, titleLbl, msgLbl),
        actions);
}

void showMsgDialog(Node &n, String title, String msg) {
    showDialog(n, msgDialog(title, msg));
}

void showMsgDialog(Node &n, String msg) {
    showDialog(n, msgDialog("Message", msg));
}

} // namespace Karm::Ui
