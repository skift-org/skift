#pragma once

#include <karm-layout/align.h>

#include "node.h"

namespace Karm::Ui {

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

template <typename T>
using DialogFn = Func<void(T)>;

template <typename T>
struct Dialog {
    DialogFn<T> fn;
};

template <typename T>
Child dialog(DialogFn<T> fn, Child inner) {
    return supplier<Dialog<T>>(inner, std::move(fn));
}

/* ---  Dialog Base  -------------------------------------------------------- */

Child dialogLayer(Child child);

void showDialog(Node &n, Child child);

template <typename T>
void showDialog(Node &n, DialogFn<T> fn, Child child) {
    showDialog(n, dialog(std::move(fn), child));
}

void closeDialog(Node &n);

/* --- Popover Base --------------------------------------------------------- */

void showPopover(Node &n, Math::Vec2i at, Child child);

/* --- Dialogs Scaffolding -------------------------------------------------- */

Child dialogScafold(Layout::Align a, Child inner);

Child dialogScafold(Layout::Align a, Child content, Children actions);

Child dialogButton(DialogButton result, bool primary = false);

Child dialogButtons(DialogButton buttons, DialogButton primary);

/* --- Dialogs -------------------------------------------------------------- */

Child aboutDialog(Media::Icons icon, String name);

void showAboutDialog(Node &n, Media::Icons icon, String name);

Child msgDialog(String title, String msg);

void showMsgDialog(Node &n, String title, String msg);

void showMsgDialog(Node &n, String msg);

} // namespace Karm::Ui
