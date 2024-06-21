#include <karm-kira/badge.h>
#include <karm-kira/dialog.h>
#include <karm-ui/box.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>

#include "dialogs.h"

namespace Hideo {

// MARK: Dialogs Scaffolding ---------------------------------------------------

Ui::Child dialogScafold(Math::Align, Ui::Child content, Ui::Children actions) {
    return Kr::dialogContent({
        content | Ui::grow(),
        hflow(8, actions) | Ui::spacing(8),
    });
}

Ui::Child dialogCloseButton() {
    return button(
        Ui::closeDialog,
        Ui::ButtonStyle::primary(),
        "Close"
    );
}

// MARK: Dialogs ---------------------------------------------------------------

Ui::Child versionBadge() {
    Ui::Children badges = {};
    badges.pushBack(Kr::badge(
        Kr::BadgeStyle::INFO,
        stringify$(__ck_version_value) ""s
    ));
#ifdef __ck_branch_nightly__
    badges.pushBack(Kr::badge(Gfx::INDIGO400, "Nightly"s));
#elif defined(__ck_branch_stable__)
    // No badge for stable
#else
    badges.pushBack(Kr::badge(Gfx::EMERALD, "Dev"s));
#endif
    return Ui::hflow(4, badges);
}

static constexpr Str LICENSE = R"(Copyright © 2018-2024, the skiftOS Developers
Copyright © 2024, Odoo S.A.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.)";

Ui::Child licenseDialog() {
    return Kr::dialogContent({
        Kr::dialogTitleBar("License"s),
        Ui::bodySmall(LICENSE) | Ui::spacing(16) | Ui::vscroll() | Ui::maxSize({480, Ui::UNCONSTRAINED}) | Ui::grow(),
    });
}

Ui::Child aboutDialog(Mdi::Icon i, String name) {
    auto content =
        Ui::vflow(
            16,
            Ui::vflow(
                8,
                Math::Align::CENTER,
                Ui::icon(i, 56) | Ui::spacing(8),
                Ui::titleLarge(name),
                versionBadge()
            ),
            Ui::text(
                Ui::TextStyles::bodySmall()
                    .withAlign(Gfx::TextAlign::CENTER)
                    .withColor(Ui::GRAY400),
                "Copyright © 2018-2024\n"
                "The skiftOS Developers & Odoo S.A.\n"
                "\n"
                "All rights reserved."
            )
        ) |
        Ui::spacing(16);

    Ui::Children actions = {
        button(
            [](auto &n) {
                showDialog(n, licenseDialog());
            },
            Ui::ButtonStyle::subtle(), Mdi::LICENSE,
            "License"
        ),
        Ui::grow(NONE),
        dialogCloseButton(),
    };

    return dialogScafold(
        Math::Align::CENTER | Math::Align::CLAMP,
        content | Ui::minSize({280, Ui::UNCONSTRAINED}),
        actions
    );
}

void showAboutDialog(Ui::Node &n, Mdi::Icon icon, String name) {
    showDialog(n, aboutDialog(icon, name));
}

Ui::Child msgDialog(String title, String msg) {
    auto titleLbl = Ui::titleMedium(title);
    auto msgLbl = Ui::text(msg);
    Ui::Children actions = {
        Ui::grow(NONE),
        button(
            Ui::closeDialog,
            Ui::ButtonStyle::primary(),
            "Ok"
        ),
    };

    return dialogScafold(
        Math::Align::CENTER,
        vflow(16, titleLbl, msgLbl),
        actions
    );
}

void showMsgDialog(Ui::Node &n, String title, String msg) {
    showDialog(n, msgDialog(title, msg));
}

void showMsgDialog(Ui::Node &n, String msg) {
    showDialog(n, msgDialog("Message"s, msg));
}

} // namespace Hideo
