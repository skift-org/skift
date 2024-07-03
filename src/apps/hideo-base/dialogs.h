#pragma once

#include <karm-math/align.h>
#include <karm-ui/dialog.h>
#include <karm-ui/node.h>

namespace Hideo {

Ui::Child licenseDialog();

Ui::Child aboutDialog(Mdi::Icon icon, String name);

Ui::Child versionBadge();

void showAboutDialog(Ui::Node &n, Mdi::Icon icon, String name);

} // namespace Hideo
