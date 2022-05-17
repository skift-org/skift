#pragma once

#include <karm-base/func.h>

namespace Karm::Ui {

struct Event {};

using EventFunc = Func<void(Event const &)>;

} // namespace Karm::Ui
