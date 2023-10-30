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

template <typename T>
inline auto dialog(DialogFn<T> fn) {
    return [fn = std::move(fn)](Child inner) {
        return dialog(std::move(fn), inner);
    };
}

/* ---  Dialog Base  -------------------------------------------------------- */

Child dialogLayer(Child child);

inline auto dialogLayer() {
    return [](Child child) {
        return dialogLayer(child);
    };
}

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

inline auto dialogScafold(Layout::Align a) {
    return [a](Child inner) {
        return dialogScafold(a, inner);
    };
}

Child dialogScafold(Layout::Align a, Child content, Children actions);

inline auto dialogScafold(Layout::Align a, Children content) {
    return [a, content = std::move(content)](Child inner) {
        return dialogScafold(a, inner, content);
    };
}

Child dialogButton(DialogButton result, bool primary = false);

Child dialogButtons(DialogButton buttons, DialogButton primary);

/* --- Dialogs -------------------------------------------------------------- */

Child aboutDialog(Mdi::Icon icon, String name);

Child versionBadge();

void showAboutDialog(Node &n, Mdi::Icon icon, String name);

Child msgDialog(String title, String msg);

void showMsgDialog(Node &n, String title, String msg);

void showMsgDialog(Node &n, String msg);

} // namespace Karm::Ui
