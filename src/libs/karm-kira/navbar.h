#pragma once

#include <karm-ui/input.h>

#include "_prelude.h"

namespace Karm::Kira {

Ui::Child navbarContent(Ui::Children children);

Ui::Child navbarItem(Ui::OnPress onPress, Mdi::Icon icon, Str text, bool selected);

} // namespace Karm::Kira
