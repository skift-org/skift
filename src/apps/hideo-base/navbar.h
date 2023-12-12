#pragma once

#include <karm-ui/input.h>

namespace Hideo {

Ui::Child navbar(Ui::Children children);

Ui::Child navbar(Meta::Same<Ui::Child> auto... children) {
    return navbar({children...});
}

Ui::Child navbarItem(Mdi::Icon icon, char const *text, bool selected, Ui::OnPress onPress);

} // namespace Hideo
