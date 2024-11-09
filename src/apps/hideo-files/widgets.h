#pragma once

#include <karm-sys/dir.h>
#include <karm-ui/node.h>

#include "model.h"

namespace Hideo::Files {

// MARK: Common Widgets --------------------------------------------------------

Ui::Child directoryListing(State const &s, Sys::Dir const &);

Ui::Child breadcrumb(State const &s);

Ui::Child goBackTool(State const &s);

Ui::Child goForwardTool(State const &s);

Ui::Child goParentTool(State const &s);

Ui::Child refreshTool();

Ui::Child moreTool(State const &s);

Ui::Child toolbar(State const &s);

Ui::Child alert(State const &s, String title, String body);

// MARK:  Dialogs  -------------------------------------------------------------

Ui::Child openFileDialog();

Ui::Child saveFileDialog();

Ui::Child directoryDialog();

} // namespace Hideo::Files
