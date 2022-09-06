#pragma once

#include <karm-sys/dir.h>
#include <karm-ui/node.h>

namespace FileManager {

/* --- Common Widgets ------------------------------------------------------- */

Ui::Child directoryListing(Sys::Dir const &dir);

Ui::Child breadcrumb();

Ui::Child toolbar();

/* ---  Dialogs  ------------------------------------------------------------ */

Ui::Child openFileDialog();

Ui::Child saveFileDialog();

Ui::Child directoryDialog();

} // namespace FileManager
