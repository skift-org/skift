#pragma once

#include <karm-ui/input.h>

#include "_prelude.h"

namespace Karm::Kira {

Ui::Child sidenav(Ui::Children children);

Ui::Child sidenavTree(Mdi::Icon icon, String title, Ui::Slot child);

Ui::Child sidenavItem(bool selected, Ui::OnPress onPress, Mdi::Icon icon, String title);

Ui::Child sidenavTitle(String title);

} // namespace Karm::Kira
