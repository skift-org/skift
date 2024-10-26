#pragma once

#include <karm-sys/dir.h>
#include <karm-ui/node.h>

#include "model.h"

namespace Hideo::Files {

// MARK: Common Widgets --------------------------------------------------------

Ui::Child directoryListing(State const &, Sys::Dir const &);

Ui::Child breadcrumb(State const &);

Ui::Child goBackTool(State const &state);

Ui::Child goForwardTool(State const &state);

Ui::Child goParentTool(State const &state);

Ui::Child refreshTool();

Ui::Child toolbar(State const &);

Ui::Child alert(State const &state, String title, String body);

// MARK:  Dialogs  -------------------------------------------------------------

Ui::Child openFileDialog();

Ui::Child saveFileDialog();

Ui::Child directoryDialog();

} // namespace Hideo::Files
