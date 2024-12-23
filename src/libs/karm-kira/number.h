#pragma once

#include <karm-ui/input.h>

#include "_prelude.h"

namespace Karm::Kira {

Ui::Child number(f64 value, Ui::OnChange<f64> onChange, f64 step = 1);

} // namespace Karm::Kira
