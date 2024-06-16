#pragma once

#include <karm-ui/input.h>

#include "_prelude.h"

namespace Karm::Kira {

Ui::Child checkbox(bool value, Ui::OnChange<bool> onChange);

} // namespace Karm::Kira
