#include "dialog.h"

#include "anim.h"
#include "box.h"
#include "drag.h"
#include "funcs.h"
#include "input.h"
#include "layout.h"
#include "scroll.h"
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
            g.fillStyle(Ui::GRAY950.withOpacity(0.75 * _opacity.value()));
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
    auto layout =
        vflow(
            8,
            content | grow(),
            hflow(8, actions)) |
        spacing(16);

    return dialogScafold(a, layout);
}

Child dialogCloseButton() {
    return button(
        closeDialog,
        ButtonStyle::primary(),
        "CLOSE");
}

/* --- Dialogs -------------------------------------------------------------- */

Child versionBadge() {
    Children badges = {};
    badges.pushBack(badge(BadgeStyle::INFO, stringify$(__ck_version_value)));
#ifdef __ck_branch_nightly__
    badges.pushBack(badge(Gfx::INDIGO400, "Nightly"));
#elif defined(__ck_branch_stable__)
#else
    badges.pushBack(badge(Gfx::EMERALD, "Dev"));
#endif
    return hflow(4, badges);
}

static constexpr Str LICENSE = R"(Copyright © 2018-2024, the skiftOS Developers

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.)";

Ui::Child licenseDialog() {
    return Ui::dialogScafold(
        Layout::Align::CLAMP |
            Layout::Align::CENTER |
            Layout::Align::TOP_START,
        Ui::vflow(
            8,
            Ui::titleLarge("License"),
            Ui::bodySmall(LICENSE) | Ui::vscroll() | Ui::maxSize({480, Ui::UNCONSTRAINED}) | Ui::grow()),
        {Ui::grow(NONE), Ui::dialogCloseButton()});
}

Child aboutDialog(Mdi::Icon i, String name) {
    auto content = vflow(
        vflow(
            8,
            Layout::Align::CENTER,
            spacing(8, icon(i, 56)),
            titleLarge(name),
            versionBadge()),
        text(
            Ui::TextStyles::bodySmall()
                .withAlign(Gfx::TextAlign::CENTER)
                .withColor(Ui::GRAY400),
            "Copyright © 2018-2024\nThe skiftOS Developers\nAll rights reserved.") |
            Ui::spacing(16));

    Children actions = {
        button(
            [](auto &n) {
                showDialog(n, licenseDialog());
            },
            ButtonStyle::subtle(), Mdi::LICENSE, "LICENSE"),
        grow(NONE),
        dialogCloseButton(),
    };

    return dialogScafold(
        Layout::Align::CENTER | Layout::Align::CLAMP,
        content | minSize({280, Ui::UNCONSTRAINED}),
        actions);
}

void showAboutDialog(Node &n, Mdi::Icon icon, String name) {
    showDialog(n, aboutDialog(icon, name));
}

Child msgDialog(String title, String msg) {
    auto titleLbl = titleMedium(title);
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
