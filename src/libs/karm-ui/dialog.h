#pragma once

#include "node.h"

namespace Karm::Ui {

/* ---  Dialog Base  -------------------------------------------------------- */

Child dialogLayer(Child child);

void showDialog(Node &n, Child child);

void closeDialog(Node &n);

/* --- Dialogs Scaffolding -------------------------------------------------- */

Ui::Child dialogScafold(Ui::Child content, Ui::Children actions);

Ui::Child dialogCloseButton();

/* --- Dialogs -------------------------------------------------------------- */

Ui::Child aboutDialog(Media::Icons icon, String name);

Ui::Child msgDialog(String title, String msg);

} // namespace Karm::Ui
