#pragma once

#include <karm-async/cancelation.h>
#include <karm-ui/node.h>

namespace Hideo::Clock {

Ui::Child app();

Async::Task<> timerTask(Ui::Child app, Async::Ct ct);

} // namespace Hideo::Clock
