#pragma once

#include <karm-layout/align.h>
#include <karm-ui/dialog.h>
#include <karm-ui/node.h>

namespace Hideo {

enum struct DialogButton {
    NONE = 0,

    OK = 1 << 0,
    CANCEL = 1 << 1,
    ABORT = 1 << 2,
    RETRY = 1 << 3,
    IGNORE = 1 << 4,
    YES = 1 << 5,
    NO = 1 << 6,
};

FlagsEnum$(DialogButton);

/* --- Dialogs Scaffolding -------------------------------------------------- */

Ui::Child dialogScafold(Layout::Align a, Ui::Child inner);

inline auto dialogScafold(Layout::Align a) {
    return [a](Ui::Child inner) {
        return dialogScafold(a, inner);
    };
}

Ui::Child dialogScafold(Layout::Align a, Ui::Child content, Ui::Children actions);

inline auto dialogScafold(Layout::Align a, Ui::Children content) {
    return [a, content = std::move(content)](Ui::Child inner) {
        return dialogScafold(a, inner, content);
    };
}

Ui::Child dialogButton(DialogButton result, bool primary = false);

Ui::Child dialogButtons(DialogButton buttons, DialogButton primary);

Ui::Child dialogCloseButton();

/* --- Dialogs -------------------------------------------------------------- */

Ui::Child licenseDialog();

Ui::Child aboutDialog(Mdi::Icon icon, String name);

Ui::Child versionBadge();

void showAboutDialog(Ui::Node &n, Mdi::Icon icon, String name);

Ui::Child msgDialog(String title, String msg);

void showMsgDialog(Ui::Node &n, String title, String msg);

void showMsgDialog(Ui::Node &n, String msg);

} // namespace Hideo