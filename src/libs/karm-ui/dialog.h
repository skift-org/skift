#pragma once

#include <karm-layout/align.h>

#include "node.h"

namespace Karm::Ui {

/* ---  Dialog Base  -------------------------------------------------------- */

Child dialogLayer(Child child);

void showDialog(Node &n, Child child);

void closeDialog(Node &n);

/* --- Dialogs Scaffolding -------------------------------------------------- */

Child dialogScafold(Child content, Children actions, Layout::Align a = Layout::Align::CENTER);

Ui::Child dialogCloseButton();

/* --- Dialogs -------------------------------------------------------------- */

Ui::Child aboutDialog(Media::Icons icon, String name);

Ui::Child msgDialog(String title, String msg);

Ui::Child openFileDialog();

Ui::Child saveFileDialog();

Ui::Child directoryDialog();

} // namespace Karm::Ui
