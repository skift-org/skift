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
struct DialogResult {
    DialogButton button;
    Opt<T> value;

    DialogResult(DialogButton b)
        : button(b) {}

    DialogResult(DialogButton b, T v)
        : button(b), value(v) {}
};

/* ---  Dialog Base  -------------------------------------------------------- */

Child dialogLayer(Child child);

void showDialog(Node &n, Child child);

void closeDialog(Node &n);

/* --- Dialogs Scaffolding -------------------------------------------------- */

Child dialogScafold(Layout::Align a, Child inner);

Child dialogScafold(Layout::Align a, Child content, Children actions);

Child dialogButton(DialogButton result, bool primary = false);

Child dialogButtons(DialogButton buttons, DialogButton primary);

/* --- Dialogs -------------------------------------------------------------- */

Child aboutDialog(Media::Icons icon, String name);

Child msgDialog(String title, String msg);

} // namespace Karm::Ui
