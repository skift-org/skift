#pragma once

#include <karm-ui/input.h>

namespace Hideo {

Ui::Child sidenav(Ui::Children children);

Ui::Child sidenavTree(Mdi::Icon icon, String title, Ui::Child child);

Ui::Child sidenavItem(bool selected, Ui::OnPress onPress, Mdi::Icon icon, String title);

} // namespace Hideo
