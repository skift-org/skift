#pragma once

#include <karm-ui/node.h>

namespace Hideo::Clock {

Ui::Child app();

Async::Task<> timerTask(Ui::Child app);

} // namespace Hideo::Clock
