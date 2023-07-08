#pragma once

#include <karm-sys/dir.h>
#include <karm-ui/node.h>

#include "model.h"

namespace FileManager {

/* --- Common Widgets ------------------------------------------------------- */

Ui::Child directoryListing(State const &, Sys::Dir const &);

Ui::Child breadcrumb(State const &);

Ui::Child toolbar(State const &);

Ui::Child alert(State const &state, String title, String subtitle);

/* ---  Dialogs  ------------------------------------------------------------ */

Ui::Child openFileDialog();

Ui::Child saveFileDialog();

Ui::Child directoryDialog();

} // namespace FileManager
