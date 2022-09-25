#include "dialog.h"

#include "box.h"
#include "button.h"
#include "drag.h"
#include "funcs.h"
#include "layout.h"
#include "proxy.h"
#include "scafold.h"
#include "view.h"

namespace Karm::Ui {

/* ---  Dialog Base  -------------------------------------------------------- */

struct DialogLayer : public Widget<DialogLayer> {
    Child _child;
    Opt<Child> _dialog;
    Opt<Child> _shouldShow;
    bool _shouldClose = false;

    DialogLayer(Child child) : _child(child) {
        _child->attach(this);
    }

    ~DialogLayer() {
        if (_dialog) {
            (*_dialog)->detach(this);
        }

        _child->detach(this);
    }

    Node &child() {
        return *_child;
    }

    Node const &child() const {
        return *_child;
    }

    Node &dialog() {
        return **_dialog;
    }

    Node const &dialog() const {
        return **_dialog;
    }

    bool visible() const {
        return (bool)_dialog;
    }

    void show(Child dialog) {
        // We need to defer showing the dialog until the next frame,
        // otherwise replacing the dialog might cause some use after free down the tree
        _shouldShow = dialog;
        shouldLayout(*this);
    }

    void close() {
        if (_dialog) {
            // We need to defer closing the dialog until the next frame,
            // otherwise we might cause some use after free down the tree
            _shouldClose = true;
            shouldLayout(*this);
        }
    }

    void reconcile(DialogLayer &o) override {
        _child = tryOr(_child->reconcile(o._child), _child);
        _child->attach(this);
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        child().paint(g, r);
        if (visible()) {
            g.fillStyle(Gfx::ZINC900.withOpacity(0.9));
            g.fill(bound());
            dialog().paint(g, r);
        }
    }

    void event(Events::Event &e) override {
        if (visible()) {
            dialog().event(e);
        } else {
            child().event(e);
        }
    }

    void layout(Math::Recti r) override {
        if (_shouldClose) {
            (*_dialog)->detach(this);
            _dialog = NONE;
            _shouldClose = false;
        }

        if (_shouldShow) {
            if (_dialog) {
                (*_dialog)->detach(this);
            }
            _dialog = _shouldShow;
            (*_dialog)->attach(this);
            _shouldShow = NONE;
        }

        child().layout(r);
        if (visible()) {
            (*_dialog)->layout(r);
        }
    }

    Math::Vec2i size(Math::Vec2i s, Layout::Hint hint) override {
        return child().size(s, hint);
    }

    Math::Recti bound() override {
        return _child->bound();
    }

    void visit(Visitor &v) override {
        v(*_child);
        if (_dialog) {
            v(**_dialog);
        }
    }
};

Child dialogLayer(Child child) {
    return makeStrong<DialogLayer>(child);
}

void showDialog(Node &n, Child child) {
    queryParent<DialogLayer>(n).show(child);
}

void closeDialog(Node &n) {
    queryParent<DialogLayer>(n).close();
}

/* --- Dialogs Scaffolding -------------------------------------------------- */

Child dialogScafold(Layout::Align a, Child inner) {
    BoxStyle boxStyle = {
        .borderRadius = 4,
        .borderWidth = 1,
        .borderColor = Gfx::ZINC700,
        .backgroundColor = Gfx::ZINC800,
    };

    return spacing(16, align(a, dragRegion(box(boxStyle, inner))));
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

Child aboutDialog(Media::Icons i, String name) {
    auto content = vflow(
        8,
        Layout::Align::CENTER,
        spacing(16, icon(i, 48)),
        text(24, name),
        empty(),
        badge(BadgeStyle::INFO, "v0.1.0"),
        empty(),
        text("Copyright © 2018-2023"),
        text("SMNX & contributors."));

    Children actions = {
        button(NONE, ButtonStyle::subtle(), "LICENSE"),
        grow(),
        dialogCloseButton(),
    };

    return dialogScafold(
        Layout::Align::CENTER | Layout::Align::CLAMP,
        content,
        actions);
}

void showAboutDialog(Node &n, Media::Icons icon, String name) {
    showDialog(n, aboutDialog(icon, name));
}

Child msgDialog(String title, String msg) {
    auto titleLbl = text(16, title);
    auto msgLbl = text(msg);
    Children actions = {
        grow(),
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
