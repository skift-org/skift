#pragma once

#include <karm-ui/input.h>

#include "_prelude.h"

namespace Karm::Kira {

Ui::Child input(Mdi::Icon icon, String placeholder, String text);

Ui::Child input(String placeholder, String text);

} // namespace Karm::Kira
